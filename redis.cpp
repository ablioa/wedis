#include "redis.h"

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

//	int allSend = 0;
	sprintf(item,"*%d%c%c",block->size,CHAR_CR,CHAR_LF);
	strcpy(buff,item);
	for(int ix = 0; ix < block->size; ix ++){

		memset(item,0,LENGTH_WORD);

		sprintf(item,"$%d%c%c%s%c%c",
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
