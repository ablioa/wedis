#include "mservice.h"

TreeNode * build_tree_node(RedisNodeType node_type){
	TreeNode * node = (TreeNode *) calloc(1,sizeof(TreeNode));
    node->level = node_type;

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

        int dbCount = 16;//atoi(dreply->bulks->items[1]->content);
        add_database_node(hodeNode,dbCount);
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
        add_data_node(selected,reply);
    }else{
        log_message("error data on KEYS request");
    }
}