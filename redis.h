#ifndef redis_h
#define redis_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <windows.h>
#include <commctrl.h>

#include "wedis.h"
#include "connection.h"

#define CHAR_SPACE ' '
#define CHAR_TAB   '\t'

#define LENGTH_WORD     256
#define LENGTH_COMMAND  1024

#define WORKING_BUFFER_SIZE 10485760

typedef enum replpy_status{
	REPLY_STATUS_PENDING,
	REPLY_STATUS_DONE
}ReplyStatus;

typedef enum{
	REDIS_UNDEFINED,
	REDIS_STRING,
	REDIS_LIST,
	REDIS_HASH,
	REDIS_SET,
	REDIS_ZSET
}DataType;

typedef enum{
	REPLY_STATUS,
	REPLY_ERROR,
	REPLY_DIGITAL,
	REPLY_BULK,
	REPLY_MULTI
}ReplyType;

typedef enum{
    CMD_SELECT,
    CMD_KEYS,
    CMD_TYPE,
    CMD_DATA,
    CMD_AUTH,
	CMD_DATABASE_COUNT,
	CMD_DELETE_KEY,
	CMD_RENAME_KEY,
	CMD_INFO_KEYSPACE,
	CMD_INFO_STATS,
	CMD_SET_STRING
}CommandType;

typedef enum redis_node_type{
    NODE_LEVEL_HOST,
    NODE_LEVEL_DATABASE,
	NODE_LEVEL_DATA
}RedisNodeType;

typedef struct redis_host_node{
	char host[255];

	HTREEITEM db_nodes[50];
	int       capacity;
	int       used;
}RedisHostNode;

typedef struct redis_database_node{
	int    dbindex;
	
	char * pattern;
	int    cursor;
	int    page_size;

	char   dbname[255];

	HTREEITEM key_nodes[50];
	int       capacity;
	int       used;
}RedisDatabaseNode;

typedef struct redis_data_node{
	char * data_key;
	char * key_length;
	
	DataType data_type;
	char     type_name[255];
	int      quantity;
}RedisDataNode;

typedef struct tree_node{
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

	struct tree_node * parent;
}TreeNode;

TreeNode * build_tree_node(TreeNode * parent,RedisNodeType nodeType);

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

RedisParams redis_build_params(int count);

void redis_add_param(RedisParams params,RedisParam param);

struct redis_bulk{
	char  * content;
	int     length;
};
typedef struct redis_bulk * RedisBulk;

struct redis_reply{
    ReplyType   type;
    ReplyStatus reply_status;

	union{
		RedisBulk    status;
    	RedisBulk    error;
    	RedisBulk    bulk;
		
    	struct redis_reply * *   bulks;
	};

	int         array_length;
};
typedef struct redis_reply * RedisReply;

struct redis_working_buffer{
	char * buffer;

	int    capacity;

	int    size;
};

typedef struct redis_working_buffer * RedisWorkingBuffer;

extern RedisWorkingBuffer redisWorkingBuffer;

typedef struct{
	int size;
	char ** items;
}ListModel;

struct keyspace_info{
    char name[255];
    int  keys;
    int  expires;
    int  avg_ttl;

    struct keyspace_info * tail;
    struct keyspace_info * next;

    int count;
};

typedef struct keyspace_info KeyspaceInfo;
typedef struct keyspace_info * Keyspace;

struct key_value_pair{
    char key[256];
    int  value;

    struct key_value_pair * tail;
    struct key_value_pair * next;

    int count;

    struct key_value_pair * * array;
};

typedef struct key_value_pair KeyValuePair;
typedef struct key_value_pair * KVPair;

typedef int (*redis_pack_handle)(unsigned char *,int);

KVPair buildKVPair();

void destroyKVPair(KVPair kv);

void setKVPair(KVPair kv,const char * text);

KVPair parseKVPair(char * buffer);

Keyspace buildKeyspaceInfo();

void setKeyspaceValue(Keyspace info,char * value);

Keyspace parseKeyspace(char * buffer);

RedisReply read_reply(char *text,int * cur,int length);

RedisReply receive_msg(RedisConnection stream);

char * buildWord(char * word,size_t length);

char  * parse_command(char * text,const size_t size);

DataType checkDataType(char * type);

int redis_read_pack(char * text,int length,redis_pack_handle handle);

int get_bulk_size(char * text, int * cur,int length);

int get_status_scope(char * text,int length);

#endif
