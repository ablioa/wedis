#include "redis.h"

#define WDEBUG

#ifdef WDEBUG
#include <windows.h>
#endif

RedisReply * read_replay(char * text){
	int cur = 0;
	char ch = text[cur++];

	RedisReply * rp = (RedisReply*)malloc(sizeof(RedisReply));
	memset(rp,0,sizeof(RedisReply));
	rp->type = -1;
	
    switch(ch){
        /** information */
		case '+':{
			rp->type=REPLY_STATUS;
			rp->status=(char*)malloc(sizeof(char) * 256);
			memset(rp->status,0,sizeof(char)*256);

			int scur = 0;
			while(text[cur] != 0x0d || text[cur+1] != 0x0a){
			    rp->status[scur++] = text[cur];
				cur++;
			}
			cur+=2;
			break;
		}
        /** error */
		case '-':{
			break;
		}
        /** number */
		case ':':{
			break;
		}
        /** bulk */
		case '$':{
			rp->type =REPLY_BULK;
			char cnt[128]={0};
			int scur = 0;
			while(text[cur] >='0' && text[cur] <= '9'){
			    cnt[scur++] = text[cur++];
			}

			cur+=2;
			int count = atoi(cnt);

// #ifdef WDEBUG
// char buff[255]={0};
// size_t len = strlen(text);
// sprintf(buff,"size:%d,len:%d",count,len);
// MessageBox(NULL,buff,"Title",MB_OK);
// #endif

			rp->bulk = (char *) malloc(sizeof(char) * count +1);
			memset(rp->bulk,0,sizeof(char) * count+1);

			for(int ix =0; ix < count; ix ++){
				rp->bulk[ix] = text[cur++];
			}

			rp->bulk[count] = 0;
			break;
		}
        /** multibulk */
		case '*':{
			rp->type=REPLY_MULTI;
			char cnt[128]={0};
			int scur = 0;
			while(text[cur] >='0' && text[cur] <= '9'){
			    cnt[scur++] = text[cur++];
			}

			cur+=3;
			int count = atoi(cnt);

			rp->bulkSize = count;
			rp->bulks = (char **) malloc(sizeof(char*) * count);

			int bulkCount=0;
			for(int ix =0; ix < count; ix ++){
				int scur = 0;
				memset(cnt,0,128);
			    while(text[cur] >='0' && text[cur] <= '9'){
					cnt[scur++] = text[cur++];
				}
				cur+=2;
				int mcount = atoi(cnt);

				char * tup = (char *) malloc(sizeof(char)*mcount+1);
				memset(tup,0,sizeof(char)*mcount+1);

				for(int jx =0; jx < mcount; jx ++){
					tup[jx]=text[cur++];
				}

				rp->bulks[bulkCount++] = tup;

				cur+=3;
			}
			break;
		}
	}

	return rp;
}

void init_command(CommandBlock * block){
    block->size = 0;
	memset(block->list,0,sizeof(char *) * LENGTH_WORD);
}


void add_command(CommandBlock * block,char * word){
	block->list[block->size] = word;
	block->size += 1;
}

char * put_command(CommandBlock * block){
	char * buff = (char *) malloc(sizeof(char) * LENGTH_COMMAND);
	char item[LENGTH_WORD]={0};

	sprintf(item,"*%d%c%c",block->size,CHAR_CR,CHAR_LF);
	strcpy(buff,item);
	for(int ix = 0; ix < block->size; ix ++){

		memset(item,0,LENGTH_WORD);

		sprintf(item,"$%ld%c%c%s%c%c",
			strlen(block->list[ix]),
			CHAR_CR,CHAR_LF,
			block->list[ix],
			CHAR_CR,CHAR_LF);

		strcpy(buff+strlen(buff),item);
	}

	return buff;
}

void free_command(CommandBlock * block){
	for(int ix =0; ix < block->size; ix ++){
	    free(block->list[ix]);
	}
}

int isSpace(char ch){
    return (ch == CHAR_SPACE) || (ch == CHAR_TAB);
}

char * parse_command(char * text,const size_t size){
	char word[LENGTH_WORD] = {0};
	int wordCur = 0;

	CommandBlock block;
	init_command(&block);

	char * start = text;
    while(*start){
		
		if(isSpace(*start)){
			add_command(&block,buildWord(word,wordCur));
			memset(word,0,LENGTH_WORD);
			wordCur = 0;
		}else{
			word[wordCur++] = *start;
		}

		start++;

		if(!*start && wordCur != 0){
			add_command(&block,buildWord(word,wordCur));
		}
	}

	char * buff = put_command(&block);
	free_command(&block);

	return buff;
}

char * buildWord(char * word,size_t length){
	char *buff = (char *)malloc(sizeof(char) * LENGTH_WORD);

	memset(buff,0,LENGTH_WORD);
	memcpy(buff,word,length);

	return buff;
}

char * build_comment(const char * text,const char * pack){
	char * buff = (char *)malloc(sizeof(char) * 1024);
	memset(buff,0,sizeof(char) * 1024);
	sprintf(buff,"# ---- %s ----\r\n%s",text,pack);
	return buff;
}