#ifndef myconnection_h
#define myconnection_h

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

struct redis_connection{
    SOCKET socket;
    struct sockaddr_in address;
};

typedef struct redis_connection * RedisConnection;

void ginit();

RedisConnection init(char * address,int port);

void sendmsg(RedisConnection stream,char * message);

void close(RedisConnection stream);

#endif
