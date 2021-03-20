#ifndef redis_h
#define redis_h

#include <windows.h>
#include <commctrl.h>

#include "log.h"

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
	REDIS_ZSET,
	REDIS_DATABASE,
	REDIS_SYSTEM
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
	int       db_count;
	int       capacity;
	int       used;

	HMENU     db_menu;
}RedisHostNode;

struct redis_connection{
    SOCKET socket;
    struct sockaddr_in address;

    char * read_buff;
    int    capacity;
    int    free_size;

    char * host;
    int    port;
};

typedef struct redis_connection * RedisConnection;

typedef struct redis_database_node{
	int    dbindex;
	
	char   pattern[255];
	int    cursor;
	int    last;
	int    page_size;

	char   dbname[255];

	HTREEITEM key_nodes[50];
	int       capacity;
	int       used;
}RedisDatabaseNode;

typedef struct redis_data_node{
	char * data_key;
	int key_length;
	
	DataType data_type;
	char     type_name[255];
	int      quantity;
}RedisDataNode;

typedef struct tree_node{
	int tid;
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

extern int GLOBAL_TREE_NODE_ID;
TreeNode * build_tree_node(TreeNode * parent,RedisNodeType nodeType);

struct redis_param{
    const char * content;
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
        RedisBulk    number;
    	struct redis_reply * *   bulks;
	};

	int         array_length;
};
typedef struct redis_reply * RedisReply;

struct redis_progress{
	int total_buffer_size;
	int free_buffer_size;

	int total_packet_size;
	int read_packet_size;

	int total_entries;
	int read_entries;
};

typedef struct redis_progress * RedisProgress;

void free_redis_reply(RedisReply reply);

struct key_value_pair{
    char key[256];

	char * value;

    struct key_value_pair * tail;
    struct key_value_pair * next;

    int count;

    struct key_value_pair * * array;
};

typedef struct key_value_pair KeyValuePair;
typedef struct key_value_pair * KVPair;

/** package handle */
typedef int (*redis_pack_handle)(unsigned char *,int);

/** data read progress handle */
typedef void (*redis_read_progress_handle)(int,int);

KVPair buildKVPair();

void destroyKVPair(KVPair kv);

void setKVPair(KVPair kv,const char * text);

KVPair parseKVPair(char * buffer);

RedisReply read_reply(char *text,int * cur,int length,redis_read_progress_handle handle);

RedisReply receive_msg(RedisConnection stream,redis_read_progress_handle handle);

char  * parse_command(char * text,const size_t size);

DataType checkDataType(char * type);

int redis_read_pack(char * text,int length,redis_pack_handle handle);

int get_bulk_size(char * text, int * cur,int length);

int get_status_scope(char * text,int length);

#endif
