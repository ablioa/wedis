// #include "redisclient.h"

// RedisClient * redisClient;

// void init(){
//     redisClient = buildRedisClient();
    
//     RedisRequest * head = buildRedisRequest();
//     head->next = NULL;
    
//     redisClient->head = head;
//     redisClient->tail = head;
// }

// void send(const RedisClient * redisClent,const char * command,const CmdType cmdType){
//     RedisRequest * request = buildRedisRequest();
//     request->cmdType = cmdType;
//     request->command = command;

//     redisClient->tail->next = request;
//     redisClient->tail = request;

//     char * cmd_data = parse_command((char *)command,256);

// 	connection_senddata(redisClent->connection,cmd_data,strlen(cmd_data),0);
// }

// void dispatch(const RedisClient * redisClent){
//     if(redisClient->head == redisClient->head){
//         // empty request
//     }

//     RedisRequest * request = redisClient->head->next;
//     redisClient->head = request->next;
//     if(request->next == redisClient->tail){
//         redisClient->tail = NULL;
//     }

// }

// RedisClient * buildRedisClient(){
//     RedisClient * ret = (RedisClient *) malloc(sizeof(RedisClient));
//     memset(ret,0,sizeof(RedisClient));

//     return ret;
// }

// RedisRequest * buildRedisRequest(){
//     RedisRequest * ret = (RedisRequest *) malloc(sizeof(RedisRequest));
//     memset(ret,0,sizeof(RedisRequest));

//     ret->next = NULL;

//     return ret;
// }


