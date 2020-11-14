#ifndef controller_h
#define controller_h

#include "main.h"
#include "redis.h"
#include "mservice.h"

void add_database_node(TreeNode * dbnode,int dbCount);

void add_data_node(TreeNode * dbnode,RedisReply rp);

void handleDataType(Task * task,DataType dataType);

void handleRedisData(Task * task,RedisReply data);

void handleKeyspace(Keyspace keyspace);

#endif 