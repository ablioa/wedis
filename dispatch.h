#ifndef dispatch_h
#define dispatch_h

#include "queue.h"

#include "controller.h"
#include "service.h"

struct wedis_dispatch_request{
    Task * task;
    RedisReply data;
};

typedef struct wedis_dispatch_request DispatchRequest;

void dispatch(Task * task,RedisReply data);

#endif
