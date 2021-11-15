#ifndef controller_h
#define controller_h

#include "main.h"
#include "redis.h"

extern int MAX_DONE;

/** update data transfer progresss */
void update_transfer_progress(int done,int total);

/** add host node to nav tree */
TreeNode * add_host_node(char * host_name,TreeNode * hostNode);

void add_database_node(TreeNode * dbnode,int dbCount);

void add_data_node(TreeNode * dbnode,RedisReply rp);

void handle_redis_data(TreeNode * datanode,RedisReply data);

#endif 
