#ifndef dispatch_h
#define dispatch_h

#include "main.h"
#include "queue.h"
#include "redis.h"

typedef struct{
    int type;
} RedisMessage;

//void dispatch(RedisMessage * message);


void dispatch(Task * task,RedisReply * data);

void sendRedisCommand(char * command,char * dataKey,char * dataType,int type);

#endif
