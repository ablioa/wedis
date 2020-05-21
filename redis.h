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

typedef struct{
	int     type;
	char *  status;
	char *  error;
	int     digital;
	char *  bulk;

	char ** bulks;
	int     bulkSize;

	char * key;
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
