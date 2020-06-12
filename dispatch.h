#ifndef dispatch_h
#define dispatch_h

#include "main.h"
#include "queue.h"
#include "redis.h"

#include "controller.h"
#include "service.h"

typedef struct{
    int type;
} RedisMessage;

void dispatch(Task * task,RedisReply * data);

#endif
