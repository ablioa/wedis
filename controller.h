#ifndef controller_h
#define controller_h

#include "main.h"
#include "redis.h"
#include "mservice.h"

void addDatabaseNode(TreeNode * dbnode,int dbCount);

void addDataNode(TreeNode * dbnode,RedisReply rp);

void handleDataType(Task * task,DataType dataType);

void handleRedisData(Task * task,RedisReply data);

void handleKeyspace(Keyspace keyspace);

#endif 