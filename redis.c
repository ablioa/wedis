#include "redis.h"

DataType checkDataType(char * type){
	if(type == NULL){
		return REDIS_UNDEFINED;
	}

	if(strcmp("string",type) == 0){
		return REDIS_STRING;
	}

	if(strcmp("list",type) == 0){
		return REDIS_LIST;
	}

	if(strcmp("hash",type) == 0){
		return REDIS_HASH;
	}

	if(strcmp("set",type) == 0){
		return REDIS_SET;
	}

	if(strcmp("zet",type) == 0){
		return REDIS_ZSET;
	}

	return REDIS_UNDEFINED;
}

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
			rp->type=REPLY_ERROR;
			rp->error=(char*)malloc(sizeof(char) * 256);
			memset(rp->error,0,sizeof(char)*256);

			int scur = 0;
			while(text[cur] != 0x0d || text[cur+1] != 0x0a){
			    rp->error[scur++] = text[cur];
				cur++;
			}
			cur+=2;
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

		sprintf(item,"$%I64d%c%c%s%c%c",
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

Keyspace buildKeyspaceInfo(){
    Keyspace info = (Keyspace)malloc(sizeof(KeyspaceInfo));
    memset(info,0,sizeof(KeyspaceInfo));

    info->tail  = NULL;
    info->next  = NULL;
    info->count = 0;
    return info;
}

void setKeyspaceValue(Keyspace info,char * value){
    char key[255] = {0};
    char * vpos = strchr(value,'=');

    strncpy(key,value,(vpos-value));

    if(strcmp(key,"expires") == 0){
        info->expires = atoi(vpos+1);
    }

    if(strcmp(key,"keys") == 0){
        info->keys = atoi(vpos+1);
    }

    if(strcmp(key,"avg_ttl") == 0){
        info->avg_ttl = atoi(vpos+1);
    }
}

Keyspace parseKeyspace(char * buffer){
    char *buf       = buffer;
    char *outer_ptr = NULL;
    char *inner_ptr = NULL;
    char *pp_ptr    = NULL;

    char * line;
    char * dbitem;
    char * vitem;

    Keyspace head = buildKeyspaceInfo();
    while ((line = strtok_r(buf, "\r\n", &outer_ptr)) != NULL){
        if(strcmp("# Keyspace",line) == 0){
            buf = NULL;
            continue;
        }

        Keyspace node = buildKeyspaceInfo();
        if(head->next == NULL){
            head->next = node;
            head->tail = node;
        }else{
            head->tail->next = node;
            head->tail = node;
        }

        head->count ++;
        while ((dbitem = strtok_r(line, ":", &inner_ptr)) != NULL){
            if(strncmp("db",dbitem,2) == 0){
                line = NULL;
                strcpy(node->name,dbitem);
                continue;
            }

            while ((vitem = strtok_r(dbitem, ",", &pp_ptr)) != NULL){
               dbitem=NULL;
               setKeyspaceValue(node,vitem);
            }

        }
    }

    return head;
}

// char buff[256] = {0};
// int main(int argc, char *argv[]){
//     FILE * file = fopen("data.txt","r");
//     fread(buff,sizeof(unsigned char),256,file);
//     fclose(file);

//     Keyspace space = parseKeyspace(buff);

//     Keyspace inode = space->next;
//     while(inode != NULL){
//         printf("#####> %s,keys=%d,expires=%d,avg_ttl=%d\n",inode->name,inode->keys,inode->expires,inode->avg_ttl);
//         inode = inode->next;
//     }

//     return 0;
// }