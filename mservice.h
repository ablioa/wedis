#ifndef mservice_h
#define mservice_h

#include "myconnection.h"
#include "redis.h"
#include "service.h"

void s_auth(RedisConnection stream,char * password);

void s_key_space(RedisConnection stream);

#endif
