#include "mservice.h"

void s_auth(TreeNode * hodeNode,char * password){
    RedisParams param = redis_auth(password);
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
    RedisParams param = redis_key_space();
    RedisReply reply = redis_serialize_params(hodeNode->stream,param);

    if(reply->type == REPLY_BULK){
        Keyspace space = parseKeyspace(reply->bulk->content);
        handleKeyspace(space);

        RedisParams param = redis_database_count();
        RedisReply dreply = redis_serialize_params(hodeNode->stream,param);

        int dbCount = atoi(dreply->bulks->items[1]->content);
        addDatabaseNode(hodeNode,dbCount);
    }
}

void s_db_select(TreeNode * selected){
    RedisParams param = redis_select(selected->database);
    RedisReply reply = redis_serialize_params(selected->stream,param);

    if(reply->type != REPLY_STATUS || strcmp("OK",reply->status->content) != 0){
        log_message("database select error");
        return;
    }

    param = redis_keys();
    reply = redis_serialize_params(selected->stream,param);

    if(reply->type == REPLY_MULTI){
        addDataNode(selected,reply);
    }else{
        log_message("error data on KEYS request");
    }
}