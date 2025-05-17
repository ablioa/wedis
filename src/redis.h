#ifndef redis_h
#define redis_h

#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>

#include "queue.h"
#include "log.h"

#define CHAR_SPACE ' '
#define CHAR_TAB   '\t'

#define LENGTH_WORD     256
#define LENGTH_COMMAND  1024

#define WORKING_BUFFER_SIZE 10485760

extern int MAX_DONE;
extern FILE * logstream;

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

struct redis_connection{
    SOCKET socket;
    struct sockaddr_in address;

    char * read_buff;
    int    capacity;
    int    free_size;

    char * host;
    int    port;

	int connection_id;
};
typedef struct redis_connection * RedisConnection;

typedef struct redis_host_node{
    char host[255];

    HTREEITEM db_nodes[50];

    /** total database count in the host*/
    int       db_count;
   
    HMENU     db_menu;

    int       capacity;

    int       used;
}RedisHostNode;

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

    /** menu to other database index*/
    HMENU  db_menu;
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

	int timer_handler;
}TreeNode;

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

extern int GLOBAL_TREE_NODE_ID;

TreeNode * build_tree_node(TreeNode * parent,RedisNodeType nodeType);
RedisParams redis_build_params(int count);
void redis_add_param(RedisParams params,RedisParam param);

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


int s_auth(TreeNode * hodeNode,char * password);

void s_key_space(TreeNode * hodeNode);

void s_add_string(TreeNode * db_node,char * key,int key_length,char * value,int value_length);

void s_db_get_data(TreeNode * dbnode,int cursor,char * pattern,int count);

void s_db_select(TreeNode * selected);

void s_handle_data(TreeNode * datanode,DataType dataType);

RedisReply s_db_flushdb(TreeNode * dbnode);

RedisReply s_db_ping(TreeNode * dbnode);

RedisReply s_db_fetch_string(TreeNode * datanode);

RedisReply s_db_fetch_hash(TreeNode * datanode);

RedisReply s_db_fetch_list(TreeNode * datanode);

RedisReply s_db_fetch_set(TreeNode * datanode);

RedisReply s_db_fetch_zset(TreeNode * datanode);

void s_db_data_type(TreeNode * selected);

RedisReply s_db_delete_key(TreeNode * dataNode,const char * data_key);

RedisReply s_db_info_stats(TreeNode * host,const char * information);

RedisReply redis_serialize_params(RedisConnection stream,RedisParams params);

/**
 * 动态参数序列化
 * 2^32最大10位+2+2+1 = 15
 **/
RedisParam redis_build_param(const char * content);
RedisParam redis_build_real_param(const char * content,int length);

void appendTask(CommandType cmdType,const DataType dataType,const char * dataKey);

RedisParams redis_delete_key(char * dataKey);

RedisReply redis_rename_key(TreeNode * dataNode,char * dataKey,char * newKey);

DataType s_db_get_data_type(const RedisConnection stream,const char * key,const int length);

/** update data transfer progresss */
void update_transfer_progress(int done,int total);

/** add host node to nav tree */
TreeNode * add_host_node(char * host_name,TreeNode * hostNode);

void add_database_node(TreeNode * dbnode,int dbCount);

void add_data_node(TreeNode * dbnode,RedisReply rp);

void handle_redis_data(TreeNode * datanode,RedisReply data);

/**/
RedisReply s_client_list(TreeNode * hostnode);
#endif
