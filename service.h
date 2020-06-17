#ifndef _service_h
#define _service_h

#include "main.h"
#include "queue.h"
#include "redis.h"

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

void redis_serialize_params(RedisParams params);

/**
 * 动态参数序列化
 * 2^32最大10位+2+2+1 = 15
 **/
RedisParam redis_build_param(char * content);

void appendTask(CommandType cmdType,const DataType dataType,const char * dataKey);

void redis_auth(char * password);

void redis_database_count();

void redis_select(int database);

void redis_data_type(char * dataKey);

void redis_get_string(char * key);

void redis_get_list(char * dataKey);

void redis_get_hash(char * dataKey);

void redis_get_set(char * dataKey);

void redis_get_zset(char * dataKey);

void redis_delete_key(char * dataKey);

void redis_rename_key(char * dataKey,char * newKey);

void redis_key_space();

void redis_keys();

void redis_info_stats();

void redis_add_big();

#endif
