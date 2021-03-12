#ifndef controller_h
#define controller_h

#include "main.h"
#include "redis.h"

void add_database_node(TreeNode * dbnode,int dbCount);

void add_data_node(TreeNode * dbnode,RedisReply rp);

void handle_redis_data(TreeNode * datanode,RedisReply data);

#endif 