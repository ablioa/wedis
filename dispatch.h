#ifndef dispatch_h
#define dispatch_h

typedef struct{
    int type;
} RedisMessage;

void dispatch(RedisMessage * message);


void sendRedisCommand(char * command,char * dataKey,char * dataType,int type);

#endif
