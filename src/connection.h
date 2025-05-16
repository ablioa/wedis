#ifndef myconnection_h
#define myconnection_h

#include <stdio.h>
#include <stdlib.h>

#include "wedis.h"
#include "redis.h"

void ginit();

RedisConnection init(char * address,int port);

void sendmsg(RedisConnection stream,char * message,int length);

char * get_real_host(const char * hostname);

void close_connection(RedisConnection stream);

#endif
