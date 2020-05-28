#include "redisclient.h"

RedisClient * redisClient;

/**
 * 构建客户端
 */
RedisClient * buildRedisClient(HWND hwnd,const char * address,int port){
    RedisClient * client = (RedisClient *) malloc(sizeof(RedisClient));
    memset(client,0,sizeof(RedisClient));

    client->address = address;
    client->port = port;

    client->connection = build_connection();
    client->hwnd = hwnd;

    return client;
}

/**
 * 连接到服务器
 */
void connect1(const RedisClient * redisClent){
    connect_to_server(redisClient->connection,redisClient->hwnd);
}

/**
 * 发送数据包
 **/
void send1(const RedisClient * redisClent,const char * command,const CmdType cmdType){
    char * cmd_data = parse_command((char *)command,256);
	connection_senddata(redisClent->connection,cmd_data,strlen(cmd_data),0);
}

void dispatch1(const RedisClient * redisClent){

}


