#ifndef _service_h
#define _service_h

#include "main.h"
#include "queue.h"
#include "redis.h"


void sendRedisRequest(CommandType cmdType,const char * cmd,const DataType dataType,const char * dataKey);

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

#endif
