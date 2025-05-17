#include "redis.h"
#include "connection.h"

#include "config.h"

#define BUFF_INCR_SIZE 4096
FILE * logstream;
int GLOBAL_TREE_NODE_ID = 100;

/** read response from redis server then parse it */
RedisReply receive_msg(RedisConnection stream,redis_read_progress_handle handle){
    char localbuff[BUFF_INCR_SIZE] = {0};
    RedisReply reply = NULL;

    stream->capacity  = BUFF_INCR_SIZE;
    stream->free_size = stream->capacity;
    stream->read_buff = (char*) calloc(stream->capacity,sizeof(char));

    do{
        if(reply){
            free_redis_reply(reply);
            reply=NULL;
        }
        int ret = recv(stream->socket, localbuff, BUFF_INCR_SIZE, 0);

        if(stream->free_size <= ret){
            stream->capacity  += BUFF_INCR_SIZE;
            stream->free_size += BUFF_INCR_SIZE;
            stream->read_buff = (char*) realloc(stream->read_buff,stream->capacity);
        }

        memcpy(stream->read_buff + ((stream->capacity - stream->free_size)),localbuff,ret);
        stream->free_size -= ret;

        int pos = 0;
        reply = read_reply(stream->read_buff,&pos, (stream->capacity - stream->free_size),handle);

    }while(reply->reply_status != REPLY_STATUS_DONE);

    wedis_log(stream->read_buff);
    free(stream->read_buff);

    return reply;
}

void free_redis_reply(RedisReply reply){
    if(!reply){
        return;
    }

    switch (reply->type){
        case REPLY_STATUS:
        case REPLY_ERROR:
        case REPLY_BULK:
            if(reply->bulk){
                if(reply->bulk->content)
                    free(reply->bulk->content);

                free(reply->bulk);
            }

            break;
        break;

        case REPLY_DIGITAL:
            // TODO ...
        break;

        case REPLY_MULTI:
            for(int i = 0; i < reply->array_length; i++){
                if(reply->bulks[i] != NULL){
                    free_redis_reply(reply->bulks[i]);
                }
            }
        break;

        default:
            break;
    }

    free(reply);
}

RedisBulk buildRedisBulk(int length){
    RedisBulk bulk = (RedisBulk)calloc(1, sizeof(struct redis_bulk));
    bulk->content = (char *)calloc(length + 1, sizeof(char));
    bulk->length = length;
    return bulk;
}

int redis_read_pack(char * text,int length,redis_pack_handle handle){
    return 0;
}

int get_status_scope(char * text,int length){
    if(text[length-2] == '\r' && text[length -1] == '\n'){
        return length - 3;
    }else{
        return -1;
    }
}

/** read reply from redis server */
RedisReply read_reply(char *text,int * cur,int length,redis_read_progress_handle handle){
    char ch = text[(*cur)++];

    RedisReply reply = (RedisReply)calloc(1, sizeof(struct redis_reply));
    switch (ch){
        /** status */
        case '+':{
            reply->type = REPLY_STATUS;
            int slen = get_status_scope(text,length);

            if(slen == -1){
                reply->reply_status = REPLY_STATUS_PENDING;
            }else{
                reply->reply_status = REPLY_STATUS_DONE;
                RedisBulk status = buildRedisBulk(slen);
                memcpy(status->content, (text + 1), slen);
                reply->status = status;
            }

            break;
        }

        /** error */
        case '-':{
            reply->type = REPLY_DIGITAL;
            int slen = get_status_scope(text,length);

            if(slen == -1){
                reply->reply_status = REPLY_STATUS_PENDING;
            }else{
                reply->reply_status = REPLY_STATUS_DONE;
                RedisBulk error = buildRedisBulk(slen);
                memcpy(error->content, (text + 1), slen);
                reply->error = error;
            }

            break;
        }

        /** number */
        case ':':{
            reply->type = REPLY_ERROR;
            int slen = get_status_scope(text,length);

            if(slen == -1){
                reply->reply_status = REPLY_STATUS_PENDING;
            }else{
                reply->reply_status = REPLY_STATUS_DONE;
                RedisBulk number = buildRedisBulk(slen);
                memcpy(number->content, (text + 1), slen);
                reply->number = number;
            }
            break;
        }

        /** bulk */
        case '$':{
            reply->type = REPLY_BULK;
            reply->reply_status = REPLY_STATUS_DONE;

            int bulk_length =  get_bulk_size(text,cur,length);
            if(bulk_length == -1){
                reply->reply_status = REPLY_STATUS_PENDING;
                break;
            }

            if(handle != NULL){
                handle(length,bulk_length);
            }

            if((*cur) + bulk_length >= length){
                reply->reply_status = REPLY_STATUS_PENDING;
                break;
            }

            reply->bulk = buildRedisBulk(bulk_length);
            memcpy(reply->bulk->content, (text + *cur), bulk_length);
            (*cur) = (*cur) + bulk_length + 2;

            break;
        }

        /** multibulk */
        case '*':{
            reply->type = REPLY_MULTI;
            reply->reply_status = REPLY_STATUS_DONE;

            reply->array_length =  get_bulk_size(text,cur,length);
            if(reply->array_length == -1){
                reply->reply_status = REPLY_STATUS_PENDING;
                break;
            }

            reply->bulks = (RedisReply *) calloc(reply->array_length,sizeof(RedisReply));
            for(int ix = 0; ix < reply->array_length; ix ++){
                RedisReply mrep = read_reply(text,cur,length,NULL);
                if(mrep->reply_status != REPLY_STATUS_DONE){
                    reply->reply_status = REPLY_STATUS_PENDING;
                    free_redis_reply(mrep);
                    break;
                }else{
                    reply->bulks[ix] = mrep;
                    if(handle != NULL){
                        handle(ix+1,reply->array_length);
                    }
                }
            }

            break;
        }
    }

    return reply;
}

int get_bulk_size(char * text, int * cur,int length){
    char cnt[20] = {0};
    int scur = 0;

    if(*cur >= length){
        return -1;
    }

    int fake_size = 0;
    while(isdigit(text[*cur])){
        if(*cur == length -1){
            fake_size = 1;
        }
        cnt[scur++] = text[(*cur)++];
    }

    if(fake_size){
        return -1;
    }

    (*cur) = (*cur)+2;

    return atoi(cnt);
}

KVPair buildKVPair(){
    return (KVPair)calloc(1, sizeof(KeyValuePair));
}

void destroyKVPair(KVPair kv){
    // TODO release the memory
}

void setKVPair(KVPair kv, const char *text){
    char *vpos = strchr(text, ':');

    strncpy(kv->key, text, (vpos - text));
    kv->value = (char *) calloc(strlen(vpos),sizeof(char));
    strcpy(kv->value,vpos+1);
}

KVPair parseKVPair(char *buffer){
    char *buf = buffer;
    char *outer_ptr = NULL;
    char *line;

    KVPair head = buildKVPair();
    while ((line = strtok_r(buf, "\r\n", &outer_ptr)) != NULL){
        if (strncmp("#", line, 1) == 0){
            buf = NULL;
            continue;
        }

        KVPair node = buildKVPair();
        if (head->next == NULL){
            head->next = node;
            head->tail = node;
        }
        else{
            head->tail->next = node;
            head->tail = node;
        }

        head->count++;
        setKVPair(node, line);
    }

    head->array = (KVPair *)malloc(sizeof(KVPair *) * head->count);
    KVPair inode = head->next;
    int idx = 0;
    while (inode != NULL){
        head->array[idx++] = inode;
        inode = inode->next;
    }

    return head;
}

DataType checkDataType(char *type){
    if (type == NULL){
        return REDIS_UNDEFINED;
    }

    if (strcmp("string", type) == 0){
        return REDIS_STRING;
    }

    if (strcmp("list", type) == 0){
        return REDIS_LIST;
    }

    if (strcmp("hash", type) == 0){
        return REDIS_HASH;
    }

    if (strcmp("set", type) == 0){
        return REDIS_SET;
    }

    if (strcmp("zset", type) == 0){
        return REDIS_ZSET;
    }

    return REDIS_UNDEFINED;
}

RedisReply redis_serialize_params(RedisConnection stream,RedisParams params){
    char head[128] = {0};

    sprintf(head,"*%d\r\n",params->param_count);
    sendmsg(stream,head,strlen(head));

    wedis_log(head);

    for(int ix = 0; ix < params->param_count; ix ++){
        char * message = params->items[ix]->diagram;
        int length = params->items[ix]->s_length;
        sendmsg(stream,message,length);
        wedis_log(params->items[ix]->diagram);
    }

    RedisReply reply = receive_msg(stream,update_transfer_progress);
    MAX_DONE = 0;
    return reply;
}

TreeNode * build_tree_node(TreeNode * parent,RedisNodeType node_type){
    TreeNode * node = (TreeNode *) calloc(1,sizeof(TreeNode));
    node->level = node_type;
    node->parent = parent;
    node->tid = GLOBAL_TREE_NODE_ID++;

    switch (node_type){
        case NODE_LEVEL_HOST:{
            node->host = (struct redis_host_node *) calloc(1,sizeof(struct redis_host_node));
            break;
        }

        case NODE_LEVEL_DATABASE:{
            node->database = (struct redis_database_node *) calloc(1,sizeof(struct redis_database_node));
            node->database->cursor = 0;
            node->database->last = 0;
            node->database->page_size = preference->db_scan_default;
            sprintf(node->database->pattern,"%s","*");
            break;
        }

        case NODE_LEVEL_DATA:{
            node->data = (struct redis_data_node *) calloc(1,sizeof(struct redis_data_node));
            break;
        }
    }
 
    return node;
}

// TODO add callback on auth down
int s_auth(TreeNode * hostNode,char * password){
    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("auth"));
    redis_add_param(param,redis_build_param(password));

    RedisReply reply = redis_serialize_params(hostNode->stream,param);

    if(reply->type == REPLY_STATUS){
        if(strcmp("OK",reply->status->content) == 0){
            return 0;
        }
    }else if(reply->type == REPLY_ERROR){
        MessageBox(NULL,reply->error->content,"title",MB_OK);
    }else {
        MessageBox(NULL,reply->error->content,"title",MB_OK);
    }

    return 1;
}

void s_add_string(TreeNode * db_node,char * key,int key_length,char * value,int value_length){
    RedisParams param = redis_build_params(3);
    redis_add_param(param,redis_build_param("set"));
    redis_add_param(param,redis_build_param(key));
    redis_add_param(param,redis_build_param(value));

    RedisReply reply = redis_serialize_params(db_node->stream,param);
    if(reply->type != REPLY_STATUS){
        log_message("add string data failed.");
    }
}

void s_key_space(TreeNode * hodeNode){
    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("info"));
    redis_add_param(param,redis_build_param("keyspace"));

    RedisReply reply = redis_serialize_params(hodeNode->stream,param);

    if(reply->type == REPLY_BULK){
        RedisParams dparam = redis_build_params(3);
        redis_add_param(dparam,redis_build_param("config"));
        redis_add_param(dparam,redis_build_param("get"));
        redis_add_param(dparam,redis_build_param("databases"));

        RedisReply dreply = redis_serialize_params(hodeNode->stream,dparam);
        int dbCount = atoi(dreply->bulks[1]->bulk->content);
        add_database_node(hodeNode,dbCount);
    }
}

void s_db_select(TreeNode * dbnode){
    char db[10] = {0};
    sprintf(db,"%d",dbnode->database->dbindex);

    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("select"));
    redis_add_param(param,redis_build_param(db));

    RedisReply reply = redis_serialize_params(dbnode->stream,param);
    if(reply->type != REPLY_STATUS || strcmp("OK",reply->status->content) != 0){
        log_message("database select error");
        return;
    }

    App->activeHost = dbnode->parent;
}

/* search database */
void s_db_get_data(TreeNode * dbnode,int cursor,char * pattern,int count){
    char cursor_buff[255] = {0};
    char pattern_buff[255] = {0};
    char count_buff[255] = {0};

    sprintf(cursor_buff,"%d",cursor);
    sprintf(pattern_buff,"%s",pattern);
    sprintf(count_buff,"%d",count);

    RedisParams keyparam = redis_build_params(6);
    redis_add_param(keyparam,redis_build_param("scan"));
    redis_add_param(keyparam,redis_build_param(cursor_buff));
    redis_add_param(keyparam,redis_build_param("match"));
    redis_add_param(keyparam,redis_build_param(pattern_buff));
    redis_add_param(keyparam,redis_build_param("count"));
    redis_add_param(keyparam,redis_build_param(count_buff));

    RedisReply kreply = redis_serialize_params(dbnode->stream,keyparam);

    if(kreply->type == REPLY_MULTI){
        add_data_node(dbnode,kreply);
    }else{
        log_message("error data on KEYS request");
    }
}

DataType s_db_get_data_type(const RedisConnection stream,const char * key,const int length){
    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("type"));
    redis_add_param(param,redis_build_real_param(key,length));

    RedisReply reply = redis_serialize_params(stream,param);
    if(reply->bulk->content == NULL || strcmp(reply->bulk->content,"none")==0){
        DumpMessage(0x0009);
        return REDIS_STRING;
    }

    return checkDataType(reply->bulk->content);
}


void s_db_data_type(TreeNode * selected){
    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("type"));
    redis_add_param(param,redis_build_real_param(selected->data->data_key,
                selected->data->key_length));

    RedisReply reply = redis_serialize_params(selected->stream,param);
    if(reply->bulk->content == NULL || strcmp(reply->bulk->content,"none")==0){
        DumpMessage(0x0009);
        return;
    }

    selected->data->data_type = checkDataType(reply->bulk->content);
    strcpy(selected->data->type_name,reply->bulk->content);
    s_handle_data(selected,selected->data->data_type);
}

/**
 * fetch corresponding type of data
 */ 
void s_handle_data(TreeNode * datanode,DataType dataType){
    RedisReply reply = NULL;
    switch (dataType){
        case REDIS_STRING:{
            reply = s_db_fetch_string(datanode);
            // TODO handle removed data
            datanode->data->quantity = reply->bulk->length;
            break;
        }

        case REDIS_HASH:{
            reply = s_db_fetch_hash(datanode);
            // TODO handle removed data
            datanode->data->quantity = reply->array_length/2;
            break;
        }

        case REDIS_LIST:{
            reply = s_db_fetch_list(datanode);
            // TODO handle removed data
            datanode->data->quantity = reply->array_length;
            break;
        }

        case REDIS_SET:{
            reply = s_db_fetch_set(datanode);
            // TODO handle removed data
            datanode->data->quantity = reply->array_length;
            break;
        }

        case REDIS_ZSET:{
            reply = s_db_fetch_zset(datanode);
            // TODO handle removed data
            datanode->data->quantity = reply->array_length/2;
            break;
        }

        default:{
            printf("hello\n");
            break;
        }
    }

    handle_redis_data(datanode,reply);
}

/* implementation of FLUSHDB command */
RedisReply s_db_flushdb(TreeNode * dbnode){
    RedisParams params = redis_build_params(1);
    redis_add_param(params,redis_build_param("flushdb"));
    return redis_serialize_params(dbnode->stream,params);
}

/* implementation of ping command */
RedisReply s_db_ping(TreeNode * hostnode){
    RedisParams params = redis_build_params(1);
    redis_add_param(params,redis_build_param("ping"));
    return redis_serialize_params(hostnode->stream,params);
}

/* implementation of 'CLIENT LIST' */
RedisReply s_client_list(TreeNode * hostnode){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("client"));
    redis_add_param(params,redis_build_param("list"));

    return redis_serialize_params(hostnode->stream,params);
}

/* query string type data,implementation of GET command */
RedisReply s_db_fetch_string(TreeNode * datanode){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("get"));

    RedisParam p = redis_build_real_param(datanode->data->data_key,
    datanode->data->key_length);
    redis_add_param(params,p);

    return redis_serialize_params(datanode->stream,params);
}

/* fetch hash data */
RedisReply s_db_fetch_hash(TreeNode * datanode){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("hgetall"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));

    return redis_serialize_params(datanode->stream,params);
}

/** fetch list data from redis server */
RedisReply s_db_fetch_list(TreeNode * datanode){
    RedisParams params = redis_build_params(4);
    redis_add_param(params,redis_build_param("lrange"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));

    return redis_serialize_params(datanode->stream,params);
}

/** fetch set data from redis server */
RedisReply s_db_fetch_set(TreeNode * datanode){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("smembers"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));

    return redis_serialize_params(datanode->stream,params);
}

RedisReply s_db_fetch_zset(TreeNode * datanode){
    RedisParams params = redis_build_params(5);
    redis_add_param(params,redis_build_param("zrange"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));
    redis_add_param(params,redis_build_param("withscores"));

    return redis_serialize_params(datanode->stream,params);
}

RedisReply s_db_info_stats(TreeNode * hostnode,const char * information){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("info"));
    redis_add_param(params,redis_build_param(information));

    RedisReply reply = redis_serialize_params(hostnode->stream,params);

    handle_redis_data(hostnode,reply);
    return reply;
}

RedisReply s_db_delete_key(TreeNode * dataNode,const char * data_key){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("del"));
    redis_add_param(params,redis_build_param(data_key));

    RedisReply reply = redis_serialize_params(dataNode->stream,params);
    handle_redis_data(dataNode,reply);
    return reply;
}

RedisReply redis_get_ttl(TreeNode * dataNode,char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("ttl"));
    redis_add_param(params,redis_build_param(dataKey));

    RedisReply reply = redis_serialize_params(dataNode->stream,params);
    handle_redis_data(dataNode,reply);
    return reply;
}

RedisReply redis_rename_key(TreeNode * dataNode,char * dataKey,char * newKey){
    RedisParams params = redis_build_params(3);
    redis_add_param(params,redis_build_param("rename"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param(newKey));

    RedisReply reply = redis_serialize_params(dataNode->stream,params);
    handle_redis_data(dataNode,reply);
    return reply;
}

RedisParams redis_build_params(int count){
    RedisParams params = (RedisParams)calloc(1,sizeof(struct redis_params));
    params->items = (RedisParam*)calloc(count,sizeof(struct redis_param));
    return params;
}

void redis_add_param(RedisParams params,RedisParam param){
    params->items[params->param_count] = param;
    params->param_count++;
}

RedisParam redis_build_param(const char * content){
    RedisParam param = (RedisParam) calloc(1,sizeof(struct redis_param));

    param->content = content;
    param->length  = strlen(content);

    param->s_length = param->length + 15;
    param->diagram = (char*)calloc(param->s_length,sizeof(char));

    sprintf(param->diagram,"$%d\r\n%s\r\n",
        param->length,
        param->content);

    param->s_length = strlen(param->diagram);
     
    return param;
}

RedisParam redis_build_real_param(const char * content,int length){
    RedisParam param = (RedisParam) calloc(1,sizeof(struct redis_param));

    param->content = content;
    param->length  = length;

    char szbuff[20] = {0};
    sprintf(szbuff,"%d",length);
    int nsize = strlen(szbuff);

    param->s_length = param->length + nsize + 5;
    param->diagram = (char*)calloc(param->s_length,sizeof(char));

    sprintf(param->diagram,"$%s\r\n",szbuff);
    int offset = strlen(param->diagram);
    memcpy(param->diagram+offset,content,length);
    memcpy(param->diagram+offset+length,"\r\n",2);

    return param;
}
