#ifndef myconnection_h
#define myconnection_h

#include <stdio.h>
#include <stdlib.h>

#include "wedis.h"
#include "redis.h"

typedef struct redis_connection_pool{
	int size;
	list con_pool;
}RedisConnectionPool;

extern RedisConnectionPool RedisCp;

void init_connection_pool();
void pool_add_connection(const RedisConnection con);
int connection_comparator(const void * a,const void * b);
void pool_remove_connection(const int connection_id);


void ginit();
void gdestroy();
RedisConnection init(char * address,int port);
void sendmsg(RedisConnection stream,char * message,int length);
char * get_real_host(const char * hostname);
void close_connection(RedisConnection stream);

#endif
