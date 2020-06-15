#ifndef dispatch_h
#define dispatch_h

#include "queue.h"

#include "controller.h"
#include "service.h"

void dispatch(Task * task,RedisReply data);

#endif
