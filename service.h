#ifndef mservice_h
#define mservice_h

#include <windows.h>
#include <commctrl.h>

#include "connection.h"
#include "redis.h"
#include "controller.h"

void s_auth(TreeNode * hodeNode,char * password);

void s_key_space(TreeNode * hodeNode);

void s_db_select(TreeNode * selected);

void s_handle_data(TreeNode * datanode,DataType dataType);

///////////////////////////

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

RedisParams redis_get_string(char * key);

RedisParams redis_get_list(char * dataKey);

RedisParams redis_get_hash(char * dataKey);

RedisParams redis_get_set(char * dataKey);

RedisParams redis_get_zset(char * dataKey);

RedisParams redis_delete_key(char * dataKey);

RedisParams redis_rename_key(char * dataKey,char * newKey);

RedisParams redis_keys();

RedisParams redis_info_stats();

#endif
