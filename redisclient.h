#ifndef redisclient_h
#define redisclient_h

#include <windows.h>

#include "connection.h"
#include "redis.h"

/**
 * 业务指定的消息类型，作为消息分发的依据
 */
typedef int CmdType;

typedef struct{
    char * command;

    CmdType cmdType;

    RedisRequest * next;
}RedisRequest;

typedef struct{
    HWND handle;

    TcpConnection * connection;

    RedisRequest * head;
    RedisRequest * tail;
}RedisClient;

extern RedisClient * redisClent;

//void addRequest(const RedisClient * redisClient,const char * cmd,const CmdType CmdType);

void send(const RedisClient * redisClient,const char * cmd,const CmdType CmdType);

void receive();

void dispatch();

void parse();

#endif