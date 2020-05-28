// #ifndef redisclient_h
// #define redisclient_h

// #include <windows.h>

// #include "connection.h"
// #include "redis.h"

// /**
//  * 业务指定的消息类型，作为消息分发的依据
//  */
// typedef int CmdType;

// // typedef struct RedisRequest{
// //     char * command;

// //     CmdType cmdType;

// //     RedisRequest * next;
// // }RedisRequest;

// typedef struct{
//     HWND hwnd;

//     char * address;

//     int port;

//     TcpConnection * connection;
// }RedisClient;

// extern RedisClient * redisClent;

// RedisClient * buildRedisClient(HWND hwnd,const char * address,int port);

// //void addRequest(const RedisClient * redisClient,const char * cmd,const CmdType CmdType);

// // RedisRequest * buildRedisRequest1();

// void send1(const RedisClient * redisClient,const char * cmd,const CmdType CmdType);

// void receive();

// void dispatch1();

// void parse();

// #endif