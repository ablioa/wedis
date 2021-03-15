#include "service.h"

FILE * logstream;

int GLOBAL_TREE_NODE_ID = 0;

// TODO 记录二进制日志
RedisReply redis_serialize_params(RedisConnection stream,RedisParams params){
    char head[128] = {0};

    sprintf(head,"*%d\r\n",params->param_count);
    sendmsg(stream,head);

    wedis_log(head);

    //fwrite(head,strlen(head),1,log_file);

    for(int ix = 0; ix < params->param_count; ix ++){
        sendmsg(stream,params->items[ix]->diagram);
        wedis_log(params->items[ix]->diagram);
    }

    return receive_msg(stream);
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
			node->database->page_size = 20;
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

void s_auth(TreeNode * hodeNode,char * password){
    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("auth"));
    redis_add_param(param,redis_build_param(password));

	RedisReply reply = redis_serialize_params(hodeNode->stream,param);

    if(reply->type == REPLY_STATUS){
        if(strcmp("OK",reply->status->content) == 0){
            s_key_space(hodeNode);
            App->activeHost = hodeNode;
        }
    }else if(reply->type == REPLY_ERROR){
        MessageBox(NULL,reply->error->content,"title",MB_OK);
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

void s_db_data_type(TreeNode * selected){
    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("type"));
    redis_add_param(param,redis_build_param(selected->data->data_key));

    RedisReply reply = redis_serialize_params(selected->stream,param);

    selected->data->data_type = checkDataType(reply->bulk->content);
    strcpy(selected->data->type_name,reply->bulk->content);
    s_handle_data(selected,selected->data->data_type);
}

void s_handle_data(TreeNode * datanode,DataType dataType){
    RedisReply reply = NULL;
	switch (dataType){
		case REDIS_STRING:{
			reply = s_db_fetch_string(datanode);
            datanode->data->quantity = reply->bulk->length;
			break;
		}

		case REDIS_HASH:{
			reply = s_db_fetch_hash(datanode);
            datanode->data->quantity = reply->array_length/2;
			break;
		}

		case REDIS_LIST:{
			reply = s_db_fetch_list(datanode);
            datanode->data->quantity = reply->array_length;
			break;
		}

		case REDIS_SET:{
			reply = s_db_fetch_set(datanode);
            datanode->data->quantity = reply->array_length;
			break;
		}

		case REDIS_ZSET:{
			reply = s_db_fetch_zset(datanode);
            datanode->data->quantity = reply->array_length/2;
			break;
		}

		default:{
			break;
		}
	}

    handle_redis_data(datanode,reply);
}

/** flushdb command */
void s_db_flushdb(TreeNode * dbnode){
    RedisParams params = redis_build_params(1);
    redis_add_param(params,redis_build_param("flushdb"));
    return redis_serialize_params(dbnode->stream,params);
}

RedisReply s_db_fetch_string(TreeNode * datanode){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("get"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));

    return redis_serialize_params(datanode->stream,params);
}

RedisReply s_db_fetch_hash(TreeNode * datanode){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("hgetall"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));

    return redis_serialize_params(datanode->stream,params);
}

RedisReply s_db_fetch_list(TreeNode * datanode){
    RedisParams params = redis_build_params(4);
    redis_add_param(params,redis_build_param("lrange"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));

    return redis_serialize_params(datanode->stream,params);
}

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

RedisParams redis_delete_key(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("del"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_rename_key(char * dataKey,char * newKey){
    RedisParams params = redis_build_params(3);
    redis_add_param(params,redis_build_param("rename"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param(newKey));

    return params;
}
