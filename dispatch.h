#ifndef dispatch_h
#define dispatch_h

typedef struct{
    int type;
} RedisMessage;

void dispatch(RedisMessage * message);

#endif
