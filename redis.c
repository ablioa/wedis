#include "redis.h"

#include <windows.h>

RedisWorkingBuffer redisWorkingBuffer;

void init_working_buffer(){
	redisWorkingBuffer = (RedisWorkingBuffer) calloc(1,sizeof(struct redis_working_buffer));
	redisWorkingBuffer->buffer = (char*) calloc(WORKING_BUFFER_SIZE,sizeof(char));
	redisWorkingBuffer->capacity = WORKING_BUFFER_SIZE;
	redisWorkingBuffer->size = 0;
}

void append_working_buffer(unsigned char * buffer,int length){
	int new_size = redisWorkingBuffer->size + length;
    int offset = redisWorkingBuffer->size;

	wedis_log("pre.append-> address:%ld,capacity:%d,size:%d,data-length:%d",
	redisWorkingBuffer->buffer,
	redisWorkingBuffer->capacity,
	redisWorkingBuffer->size,
	length);

	if( new_size > redisWorkingBuffer->capacity){
		realloc(redisWorkingBuffer->buffer,new_size);
		redisWorkingBuffer->capacity = new_size;
	}

	memcpy(redisWorkingBuffer->buffer + offset,buffer,length);
	redisWorkingBuffer->size = new_size;

	wedis_log("post.append-> address:%ld,capacity:%d,size:%d,data-length:%d",
	redisWorkingBuffer->buffer,
	redisWorkingBuffer->capacity,
	redisWorkingBuffer->size,
	length);
}

void retain_working_buffer(int invalid_size){
	int valid_size = redisWorkingBuffer->size - invalid_size;
	unsigned char * buff = (unsigned char*)calloc((valid_size),sizeof(unsigned char));

	wedis_log("pre.retain-> address:%ld,capacity:%d,size:%d,data-length:%d",
	redisWorkingBuffer->buffer,
	redisWorkingBuffer->capacity,
	redisWorkingBuffer->size,
	valid_size);
	
	memcpy(buff,redisWorkingBuffer->buffer+invalid_size,valid_size);
	memset(redisWorkingBuffer->buffer,0,redisWorkingBuffer->capacity);
	memcpy(redisWorkingBuffer->buffer,buff,valid_size);

	redisWorkingBuffer->size = valid_size;

	wedis_log("post.retain-> address:%ld,capacity:%d,size:%d,data-length:%d",
	redisWorkingBuffer->buffer,
	redisWorkingBuffer->capacity,
	redisWorkingBuffer->size,
	valid_size);

	free(buff);
}

int getStatusReplyLength(char *text){
	int cur = 0;

	int length = 0;
	while (text[cur] != 0x0d || text[cur + 1] != 0x0a){
		cur++;
		length++;
	}

	return length - 1;
}

RedisBulk buildRedisBulk(int length){
	RedisBulk bulk = (RedisBulk)calloc(1, sizeof(struct redis_bulk));
	bulk->content = (char *)calloc(length + 1, sizeof(char));
	bulk->length = length;
	return bulk;
}

RedisBulks buildRedisBulks(int count){
	RedisBulks bulks = (RedisBulks)calloc(1, sizeof(struct redis_bulks));
	bulks->items = (RedisBulk *)calloc(count, sizeof(RedisBulk));
	bulks->count = count;
	return bulks;
}

///char * chunk;

int crlf_end(char * text,int pos){
	return (text[pos-1] == 0x0d && text[pos] == 0x0a);
}

int check_tail(char * text,int length){
	int bound = 0;
	int tail_position = 0;

	int cur = 0;
	char ch = text[0];

	wedis_log("start token(text):%s",text);
	wedis_log("start token(char):|%c|",ch);
	wedis_log("length of token:%d",length);

	switch (ch){
		case '+':
		case '-':{
			while(!crlf_end(text,cur++)){
				if(cur >= length){
					return 0;
				}
			}
			return cur;
		}

		case ':':{
			break;
		}

		case '$':{
			//int found = 1;
			char cnt[128] = {0};
			int scur = 0;
			cur++;
			while (isdigit(text[cur])){
				cnt[scur++] = text[cur++];
				if(cur >= length){return 0;}
			}

			cur += 2;
			if(cur >= length){return 0;}

			int count = atoi(cnt);
			cur += count;
			cur += 2;

			if(cur > length){return 0;}

			return cur;
		}

		case '*':{
			char cnt[128] = {0};
			cur++;
			int scur = 0;
			while (isdigit(text[cur])){
				cnt[scur++] = text[cur++];
				if(cur >= length){return 0;}
			}

			int count = atoi(cnt);
			cur += 3;
			if(cur >= length){return 0;}

			for (int ix = 0; ix < count; ix++){
				int scur = 0;
				memset(cnt, 0, 128);

				while (isdigit(text[cur])){
					cnt[scur++] = text[cur++];
					if(cur >= length){return 0;}
				}

				cur += 2;
				if(cur >= length){return 0;}
				int mcount = atoi(cnt);
				cur += mcount;
				if(cur >= length){return 0;}

				cur += 3;
				if(cur >= length){return 0;}
			}

			return cur;
		}

		default:{
			wedis_log("wrong pack: %d-> %s",length,text);
			return 0;
		}
	}
}

int redis_read_pack(char * text,int length,redis_pack_handle handle){
	append_working_buffer(text,length);
	int pos     = check_tail(redisWorkingBuffer->buffer,redisWorkingBuffer->size);
	char * next = redisWorkingBuffer->buffer;
	while(pos != 0){
		char * pk = (char*) calloc(pos,sizeof(char));
		memcpy(pk,next,pos-1);

		wedis_log("data retrieved: %d",pos-1);
		handle(pk,pos-1);

		next = redisWorkingBuffer->buffer + pos;
		if(length - pos == 0){
			pos = 0;
		}else{
			pos = check_tail(next,length - pos);
		}
	}

	wedis_log("post.process: buff: %ld,invalid-pos: %ld ,size:%d",redisWorkingBuffer->buffer,next,redisWorkingBuffer->size);
	if(next <= redisWorkingBuffer->buffer + redisWorkingBuffer->size){
		retain_working_buffer((next-redisWorkingBuffer->buffer));
	}

	return 0;
}

RedisReply read_replay(char *text,int length){
	int cur = 0;
	char ch = text[cur++];

	int consumed = 0;

	RedisReply reply = (RedisReply)calloc(1, sizeof(RedisReplyInfo));
	switch (ch){
		/** status */
		case '+':{
			reply->type = REPLY_STATUS;
			int length = getStatusReplyLength(text);

			RedisBulk status = buildRedisBulk(length);
			memcpy(status->content, (text + 1), length);
			reply->status = status;

			consumed = length + 3;
			break;
		}
		
		/** error */
		case '-':{
			reply->type = REPLY_ERROR;
			int length = getStatusReplyLength(text);

			RedisBulk error = buildRedisBulk(length);
			memcpy(error->content, (text + 1), length);
			reply->error = error;

			consumed = length + 3;
			break;
		}
		
		/** number */
		case ':':{
			break;
		}

		/** bulk */
		case '$':{
			reply->type = REPLY_BULK;
			char cnt[128] = {0};
			int scur = 0;
			while (isdigit(text[cur])){
				cnt[scur++] = text[cur++];
			}

			cur += 2;
			int count = atoi(cnt);
			if(strlen(text+cur) < count){
				char buff[256] = {0};
				sprintf(buff,"acture.size: %d,supporsed.count:%d",strlen(text+cur),count);
				MessageBox(NULL,buff,"Title",MB_OK);
			}

			RedisBulk bulk = buildRedisBulk(count);
			reply->bulk = bulk;
			memcpy(bulk->content, (text + cur), count);

			consumed = cur + count + 2;

			break;
		}
		
		/** multibulk */
		case '*':{
			reply->type = REPLY_MULTI;
			char cnt[128] = {0};
			int scur = 0;
			while (isdigit(text[cur])){
				cnt[scur++] = text[cur++];
			}

			int count = atoi(cnt);
			RedisBulks bulks = buildRedisBulks(count);
			reply->bulks = bulks;

			cur += 3;
			for (int ix = 0; ix < count; ix++){
				int scur = 0;
				memset(cnt, 0, 128);

				while (isdigit(text[cur])){
					cnt[scur++] = text[cur++];
				}

				cur += 2;
				int mcount = atoi(cnt);

				RedisBulk bulk = buildRedisBulk(mcount);
				reply->bulks->items[ix] = bulk;
				bulk->length = mcount;

				memcpy(bulk->content, (text + cur), mcount);
				cur += mcount;

				cur += 3;
			}

			consumed = cur-1;
			break;
		}
	}

	reply->consumed = consumed;

	return reply;
}

char *buildWord(char *word, size_t length){
	char *buff = (char *)malloc(sizeof(char) * LENGTH_WORD);

	memset(buff, 0, LENGTH_WORD);
	memcpy(buff, word, length);

	return buff;
}

char *build_comment(const char *text, const char *pack){
	char *buff = (char *)malloc(sizeof(char) * 1024);
	memset(buff, 0, sizeof(char) * 1024);
	sprintf(buff, "# ---- %s ----\r\n%s", text, pack);
	return buff;
}

Keyspace buildKeyspaceInfo(){
	Keyspace info = (Keyspace)malloc(sizeof(KeyspaceInfo));
	memset(info, 0, sizeof(KeyspaceInfo));

	info->tail = NULL;
	info->next = NULL;
	info->count = 0;
	return info;
}

void setKeyspaceValue(Keyspace info, char *value){
	char key[255] = {0};
	char *vpos = strchr(value, '=');

	strncpy(key, value, (vpos - value));

	if (strcmp(key, "expires") == 0){
		info->expires = atoi(vpos + 1);
	}

	if (strcmp(key, "keys") == 0){
		info->keys = atoi(vpos + 1);
	}

	if (strcmp(key, "avg_ttl") == 0){
		info->avg_ttl = atoi(vpos + 1);
	}
}

Keyspace parseKeyspace(char *buffer){
	char *buf = buffer;
	char *outer_ptr = NULL;
	char *inner_ptr = NULL;
	char *pp_ptr = NULL;

	char *line;
	char *dbitem;
	char *vitem;

	Keyspace head = buildKeyspaceInfo();
	while ((line = strtok_r(buf, "\r\n", &outer_ptr)) != NULL){
		if (strcmp("# Keyspace", line) == 0){
			buf = NULL;
			continue;
		}

		Keyspace node = buildKeyspaceInfo();
		if (head->next == NULL){
			head->next = node;
			head->tail = node;
		}
		else{
			head->tail->next = node;
			head->tail = node;
		}

		head->count++;
		while ((dbitem = strtok_r(line, ":", &inner_ptr)) != NULL){
			if (strncmp("db", dbitem, 2) == 0){
				line = NULL;
				strcpy(node->name, dbitem);
				continue;
			}

			while ((vitem = strtok_r(dbitem, ",", &pp_ptr)) != NULL){
				dbitem = NULL;
				setKeyspaceValue(node, vitem);
			}
		}
	}

	return head;
}

KVPair buildKVPair(){
	return (KVPair)calloc(1, sizeof(KeyValuePair));
}

void destroyKVPair(KVPair kv){
	// TODO 释放数据
	// TODO 释放数据索引
	// TODO 释放自己
}

void setKVPair(KVPair kv, const char *text){
	char *vpos = strchr(text, ':');

	strncpy(kv->key, text, (vpos - text));
	kv->value = atoi(vpos + 1);
}

KVPair parseKVPair(char *buffer){
	char *buf = buffer;
	char *outer_ptr = NULL;
	char *line;

	KVPair head = buildKVPair();
	while ((line = strtok_r(buf, "\r\n", &outer_ptr)) != NULL){
		if (strncmp("#", line, 1) == 0){
			buf = NULL;
			continue;
		}

		KVPair node = buildKVPair();
		if (head->next == NULL){
			head->next = node;
			head->tail = node;
		}
		else{
			head->tail->next = node;
			head->tail = node;
		}

		head->count++;
		setKVPair(node, line);
	}

	head->array = (KVPair *)malloc(sizeof(KVPair *) * head->count);
	KVPair inode = head->next;
	int idx = 0;
	while (inode != NULL){
		head->array[idx++] = inode;
		inode = inode->next;
	}

	return head;
}

DataType checkDataType(char *type){
	if (type == NULL){
		return REDIS_UNDEFINED;
	}

	if (strcmp("string", type) == 0){
		return REDIS_STRING;
	}

	if (strcmp("list", type) == 0){
		return REDIS_LIST;
	}

	if (strcmp("hash", type) == 0){
		return REDIS_HASH;
	}

	if (strcmp("set", type) == 0){
		return REDIS_SET;
	}

	if (strcmp("zset", type) == 0){
		return REDIS_ZSET;
	}

	return REDIS_UNDEFINED;
}