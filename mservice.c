#include "mservice.h"

void s_auth(RedisConnection stream,char * password){
    RedisParams param = redis_auth(password);
	RedisReply reply = redis_serialize_params(stream,param);

    if(reply->type == REPLY_STATUS){
        if(strcmp("OK",reply->status->content) == 0){
            s_key_space(stream);
        }
    }else if(reply->type == REPLY_ERROR){
        MessageBox(NULL,reply->error->content,"title",MB_OK);
    }
}

void s_key_space(RedisConnection stream){
    RedisParams param = redis_key_space();
    RedisReply reply = redis_serialize_params(stream,param);

    if(reply->type == REPLY_BULK){
        Keyspace space = parseKeyspace(reply->bulk->content);
        handleKeyspace(space);

        RedisParams param = redis_database_count();
        RedisReply dreply = redis_serialize_params(stream,param);

        int dbCount = atoi(dreply->bulks->items[1]->content);
        addDatabaseNode(dbCount);
    }
}

void s_db_select(RedisConnection stream,int database){
    RedisParams param = redis_select(database);
    RedisReply reply = redis_serialize_params(stream,param);

    if(reply->type != REPLY_STATUS || strcmp("OK",reply->status->content) != 0){
        log_message("database select error");
        return;
    }

    param = redis_keys();
    reply = redis_serialize_params(stream,param);

    if(reply->type == REPLY_MULTI){
        addDataNode(reply);
    }else{
        log_message("error data on KEYS request");
    }
}