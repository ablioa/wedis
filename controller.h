#ifndef controller_h
#define controller_h

#include "main.h"
#include "redis.h"

void addDatabaseNode(int dbCount);

void addDataNode(RedisReply rp);

void handleDataType(Task * task,DataType dataType);

void handleRedisData(Task * task,RedisReply data);

void handleKeyspace(Keyspace keyspace);

#endif 