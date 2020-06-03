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

typedef struct{
	int    size;
	char * list[LENGTH_WORD];
}CommandBlock;

#define	REPLY_STATUS  0
#define	REPLY_ERROR   1
#define	REPLY_DIGITAL 2
#define	REPLY_BULK    3
#define	REPLY_MULTI   4

/**
 * 数据类型
 */ 
#define TYPE_STRING "string"
#define TYPE_LIST   "list"
#define TYPE_HASH   "hash"
#define TYPE_SET    "set"
#define TYPE_ZSET   "zset"

typedef struct {
    int type;   // 类型

	int status; // 解析中|解析完成
}ReceiveSession;

typedef struct{
	int     type;
	char *  status;
	char *  error;
	int     digital;
	char *  bulk;

	char ** bulks;
	int     bulkSize;

	char * key;
	char * dataType;
}RedisReply;

/**
 * redis 列表数据
 */
typedef struct{
	int size;
	char ** items;
}ListModel;

RedisReply * read_replay(char * text);

void init_command(CommandBlock * block);

void add_command(CommandBlock * block,char * word);

char * put_command(CommandBlock * block);

void free_command(CommandBlock * block);

char * buildWord(char * word,size_t length);

int isSpace(char ch);

char  * parse_command(char * text,const size_t size);

#endif
