#include "service.h"

TreeNode * build_tree_node(TreeNode * parent,RedisNodeType node_type){
	TreeNode * node = (TreeNode *) calloc(1,sizeof(TreeNode));
    node->level = node_type;
    node->parent = parent;

    switch (node_type){
        case NODE_LEVEL_HOST:{
            node->host = (struct redis_host_node *) calloc(1,sizeof(struct redis_host_node));
            break;
        }

        case NODE_LEVEL_DATABASE:{
            node->database = (struct redis_database_node *) calloc(1,sizeof(struct redis_database_node));
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
        Keyspace space = parseKeyspace(reply->bulk->content);
        handleKeyspace(space);

        RedisParams dparam = redis_build_params(3);
        redis_add_param(dparam,redis_build_param("config"));
        redis_add_param(dparam,redis_build_param("get"));
        redis_add_param(dparam,redis_build_param("databases"));

        RedisReply dreply = redis_serialize_params(hodeNode->stream,dparam);

        // TODO @response format
        int dbCount = atoi(dreply->bulks[1]->bulk->content);
        add_database_node(hodeNode,dbCount);
    }
}

void s_db_select(TreeNode * selected){
    char db[10] = {0};
    sprintf(db,"%d",selected->database->dbindex);

    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("select"));
    redis_add_param(param,redis_build_param(db));

    //RedisParams param = redis_select(selected->database->dbindex);
    RedisReply reply = redis_serialize_params(selected->stream,param);

    if(reply->type != REPLY_STATUS || strcmp("OK",reply->status->content) != 0){
        log_message("database select error");
        return;
    }

    param = redis_keys();
    reply = redis_serialize_params(selected->stream,param);

    if(reply->type == REPLY_MULTI){
        add_data_node(selected,reply);
    }else{
        log_message("error data on KEYS request");
    }
}

void s_db_data_type(TreeNode * selected){
    RedisParams param = redis_build_params(2);
    redis_add_param(param,redis_build_param("type"));
    redis_add_param(param,redis_build_param(selected->data->data_key));

    RedisReply reply = redis_serialize_params(selected->stream,param);

    DataType dataType = checkDataType(reply->bulk->content);
    s_handle_data(selected,dataType);
}

void s_db_fetch_string(TreeNode * datanode){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("get"));
    redis_add_param(params,redis_build_param(datanode->data->data_key));

    RedisReply reply = redis_serialize_params(datanode->stream,params);

    // TODO process string values @reply->bulk->content
}

void s_handle_data(TreeNode * datanode,DataType dataType){
    char name [640] = {0};
    sprintf(name,": %d / %s",dataType,datanode->data->data_key);
    // MessageBox(NULL,name,"OK",MB_OK);

	switch (dataType){
		case REDIS_STRING:{
			s_db_fetch_string(datanode);
			break;
		}

		case REDIS_HASH:{
			//redis_get_hash(task->dataKey);
			break;
		}

		case REDIS_LIST:{
			//redis_get_list(task->dataKey);
			break;
		}

		case REDIS_SET:{
			//redis_get_set(task->dataKey);
			break;
		}

		case REDIS_ZSET:{
			//redis_get_zset(task->dataKey);
			break;
		}

		default:{
			//log_message("undefined data type");
			break;
		}
	}
}

RedisParams redis_build_params(int count){
    RedisParams params = (RedisParams)calloc(1,sizeof(struct redis_params));
    params->items = (RedisParam*)calloc(count,sizeof(struct redis_param));
}

void redis_add_param(RedisParams params,RedisParam param){
    params->items[params->param_count] = param;
    params->param_count++;
}

/**
 * 动态参数序列化
 * 2^32最大10位+2+2+1 = 15
 **/
RedisParam redis_build_param(char * content){
    RedisParam param = (RedisParam) calloc(1,sizeof(struct redis_param));

    param->content = content;
    param->length  = strlen(content);

    param->s_length = param->length + 15;
    param->diagram = (char*)calloc(param->s_length,sizeof(char));

    sprintf(param->diagram,"$%d%c%c%s%c%c",
        param->length,
        CHAR_CR, CHAR_LF,
        param->content,
        CHAR_CR, CHAR_LF);

    param->s_length = strlen(param->diagram);
    
    return param;
}

RedisReply redis_serialize_params(RedisConnection stream,RedisParams params){
    char head[128] = {0};
    sprintf(head,"*%d%c%c",params->param_count,CHAR_CR, CHAR_LF);
    sendmsg(stream,head);

    for(int ix = 0; ix < params->param_count; ix ++){
        char * cmd = params->items[ix]->diagram;
        int    len = params->items[ix]->s_length;
        sendmsg(stream,cmd);
    }

    return receive_msg(stream);
}

RedisParams redis_keys(){
    RedisParams params = redis_build_params(6);
    redis_add_param(params,redis_build_param("scan"));
    redis_add_param(params,redis_build_param("1"));
    redis_add_param(params,redis_build_param("match"));
    redis_add_param(params,redis_build_param("*"));
    redis_add_param(params,redis_build_param("count"));
    redis_add_param(params,redis_build_param("20"));

    return params;
}

RedisParams redis_get_string(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("get"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_get_list(char * dataKey){
    RedisParams params = redis_build_params(4);
    redis_add_param(params,redis_build_param("lrange"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));

    return params;
}

RedisParams redis_get_hash(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("hgetall"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_get_set(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("smembers"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_get_zset(char * dataKey){
    RedisParams params = redis_build_params(5);
    redis_add_param(params,redis_build_param("zrange"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));
    redis_add_param(params,redis_build_param("withscores"));

    return params;
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

RedisParams redis_info_stats(){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("info"));
    redis_add_param(params,redis_build_param("stats"));

    return params;
}

void appendTask(CommandType cmdType,const DataType dataType,const char * dataKey){
    Task * task = buildTask(cmdType);

    task->dataType = dataType;
    task->dataKey = NULL;
    if(dataKey != NULL){
        task->dataKey = (char *) malloc(sizeof(char) * 256);
        memset(task->dataKey,0,sizeof(char ) * 256);
        sprintf(task->dataKey,"%s",dataKey);
    }

    addTask(pool,task);
}
