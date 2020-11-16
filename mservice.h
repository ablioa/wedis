#ifndef mservice_h
#define mservice_h

#include <windows.h>
#include <commctrl.h>

#include "myconnection.h"
#include "redis.h"
#include "service.h"

typedef enum redis_node_type{
    NODE_LEVEL_HOST,
    NODE_LEVEL_DATABASE,
	NODE_LEVEL_DATA
}RedisNodeType;

typedef struct redis_host_node{
	char host[255];
	int  port;

	HTREEITEM db_nodes[50];
	int       capacity;
	int       used;
};

typedef struct redis_database_node{
	int    database;
	char * pattern;
	int    cursor;
	int    page_size;

	HTREEITEM key_nodes[50];
	int       capacity;
	int       used;
};

typedef struct redis_data_node{
	char * data_key;
	char * key_length;
	
	int data_type;
};

typedef struct{
	int level;

	HTREEITEM handle;
	RedisConnection stream;

	HTREEITEM subHandles[1024];
    int  subHandleSize;

	union{
		struct redis_host_node     * host;
		struct redis_database_node * database;
		struct redis_data_node     * data;
	};
}TreeNode;

TreeNode * build_tree_node(RedisNodeType nodeType);

void s_auth(TreeNode * hodeNode,char * password);

void s_key_space(TreeNode * hodeNode);

void s_db_select(TreeNode * selected);

#endif
