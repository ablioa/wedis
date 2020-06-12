#ifndef redis_h
#define redis_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHAR_SPACE ' '
#define CHAR_TAB   '\t'
#define CHAR_CR    0x0D
#define CHAR_LF    0x0A

#define LENGTH_WORD     256
#define LENGTH_COMMAND  1024

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
	CMD_INFO_STATS
}CommandType;

typedef struct{
	int    size;
	char * list[LENGTH_WORD];
}CommandBlock;

// TODO 结构要用union优化一下
typedef struct{
	int     type;
	char *  status;
	char *  error;
	int     digital;
	char *  bulk;

	char ** bulks;
	int     bulkSize;

    /** 业务数据 */
	char *   dataKey;
	char *   dataTypeName;
	DataType dataType;
}RedisReply;

/**
 * redis 列表数据
 */
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

Keyspace buildKeyspaceInfo();

void setKeyspaceValue(Keyspace info,char * value);

Keyspace parseKeyspace(char * buffer);

RedisReply * read_replay(char * text);

void init_command(CommandBlock * block);

void add_command(CommandBlock * block,char * word);

char * put_command(CommandBlock * block);

void free_command(CommandBlock * block);

char * buildWord(char * word,size_t length);

int isSpace(char ch);

char  * parse_command(char * text,const size_t size);

DataType checkDataType(char * type);

#endif
