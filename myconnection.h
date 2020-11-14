#ifndef myconnection_h
#define myconnection_h

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "redis.h"

struct redis_connection{
    SOCKET socket;
    struct sockaddr_in address;

    char * read_buff;
    int    capacity;
    int    free_size;
};

typedef struct redis_connection * RedisConnection;

void ginit();

RedisConnection init(char * address,int port);

void sendmsg(RedisConnection stream,char * message);

RedisReply receive_msg(RedisConnection stream);

void close(RedisConnection stream);

#endif
