#ifndef _service_h
#define _service_h

#include "queue.h"
#include "redis.h"
#include "myconnection.h"

#include <stdlib.h>

struct redis_param{
    char * content;
    int    length;

    char * diagram;
    int    s_length;
};
typedef struct redis_param * RedisParam;

struct redis_params{
    int param_count;
    RedisParam * items;
};
typedef struct redis_params * RedisParams;

struct command{
    char * cmd;
    
    int length;

    int capacity;
};

typedef struct comamnd * RedisComamnd;

RedisParams redis_build_params(int count);

void redis_add_param(RedisParams params,RedisParam param);

RedisReply redis_serialize_params(RedisConnection stream,RedisParams params);

/**
 * 动态参数序列化
 * 2^32最大10位+2+2+1 = 15
 **/
RedisParam redis_build_param(char * content);

void appendTask(CommandType cmdType,const DataType dataType,const char * dataKey);

RedisParams redis_auth(char * password);

RedisParams redis_database_count();

RedisParams redis_select(int database);

RedisParams redis_data_type(char * dataKey);

RedisParams redis_get_string(char * key);

RedisParams redis_get_list(char * dataKey);

RedisParams redis_get_hash(char * dataKey);

RedisParams redis_get_set(char * dataKey);

RedisParams redis_get_zset(char * dataKey);

RedisParams redis_delete_key(char * dataKey);

RedisParams redis_rename_key(char * dataKey,char * newKey);

RedisParams redis_key_space();

RedisParams redis_keys();

RedisParams redis_info_stats();

// RedisParams redis_add_big();

#endif
