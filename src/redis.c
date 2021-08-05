#include "redis.h"

#define BUFF_INCR_SIZE 4096

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

            // TODO 处理复合结构下的指针移动

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

            // TODO 处理复合结构下的指针移动

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
