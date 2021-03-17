#ifndef mservice_h
#define mservice_h

#include "wedis.h"
#include "redis.h"
#include "connection.h"
#include "controller.h"

extern FILE * logstream;

void s_auth(TreeNode * hodeNode,char * password);

void s_key_space(TreeNode * hodeNode);

void s_db_get_data(TreeNode * dbnode,int cursor,char * pattern,int count);

void s_db_select(TreeNode * selected);

void s_handle_data(TreeNode * datanode,DataType dataType);

void s_db_flushdb(TreeNode * dbnode);

RedisReply s_db_fetch_string(TreeNode * datanode);

RedisReply s_db_fetch_hash(TreeNode * datanode);

RedisReply s_db_fetch_list(TreeNode * datanode);

RedisReply s_db_fetch_set(TreeNode * datanode);

RedisReply s_db_fetch_zset(TreeNode * datanode);

void s_db_data_type(TreeNode * selected);

RedisReply s_db_info_stats(TreeNode * host,const char * information);

RedisReply redis_serialize_params(RedisConnection stream,RedisParams params);

/**
 * 动态参数序列化
 * 2^32最大10位+2+2+1 = 15
 **/
RedisParam redis_build_param(const char * content);

void appendTask(CommandType cmdType,const DataType dataType,const char * dataKey);

RedisParams redis_delete_key(char * dataKey);

RedisParams redis_rename_key(char * dataKey,char * newKey);

#endif
