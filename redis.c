#include "redis.h"

#include <windows.h>

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

/**
 * 回调方式处理报文 
 * 
 **/
int redis_read_pack(char * text,int length,redis_pack_handle handle){
	return 0;
}

/**
 * Status包围内容范围计算
 * +OK\r\n
 */
int get_status_scope(char * text,int length){
	if(text[length-2] == '\r' && text[length -1] == '\n'){
		return length - 3;
	}else{
		return -1;
	}
}

/**
 *  解析交互报文，可能失败
 **/
RedisReply read_reply(char *text,int length){
	int cur = 0;
	char ch = text[cur++];

	RedisReply reply = (RedisReply)calloc(1, sizeof(RedisReplyInfo));
	switch (ch){
		/** status */
		case '+':{
			reply->type = REPLY_STATUS;
			int slen = get_status_scope(text,length);

			if(slen == -1){
				reply->reply_status = REPLY_STATUS_PENDING;
			}else{
				reply->reply_status = REPLY_STATUS_DONE;
				RedisBulk status = buildRedisBulk(slen);
				memcpy(status->content, (text + 1), slen);
				reply->status = status;
			}

			break;
		}
		
		/** error */
		case '-':{
			reply->type = REPLY_ERROR;
			int slen = get_status_scope(text,length);

			if(slen == -1){
				reply->reply_status = REPLY_STATUS_PENDING;
			}else{
				reply->reply_status = REPLY_STATUS_DONE;
				RedisBulk error = buildRedisBulk(slen);
				memcpy(error->content, (text + 1), slen);
				reply->error = error;
			}

			break;
		}
		
		/** number */
		case ':':{
			MessageBox(NULL,"尚未支持的协议！！","sd",MB_OK);
			break;
		}

		/** bulk */
		case '$':{
			reply->type = REPLY_BULK;
			reply->reply_status = REPLY_STATUS_DONE;

			int count =  get_bulk_size(text,&cur,length);
			if(count == -1){
				reply->reply_status = REPLY_STATUS_PENDING;
			}else{
				reply->reply_status = REPLY_STATUS_DONE;
				reply->bulk = buildRedisBulk(count);

				cur += 2;
				memcpy(reply->bulk->content, (text + cur), count);
			}

			break;
		}
		
		/** multibulk */
		case '*':{
			reply->type = REPLY_MULTI;
			reply->reply_status = REPLY_STATUS_DONE;

			int count =  get_bulk_size(text,&cur,length);
			if(count == -1){
				reply->reply_status = REPLY_STATUS_PENDING;
				break;
			}

			RedisBulks bulks = buildRedisBulks(count);
			reply->bulks = bulks;

			cur += 3;
			for (int ix = 0; ix < count; ix++){
				
				int mcount =  get_bulk_size(text,&cur,length);
				if(mcount == -1){
					reply->reply_status = REPLY_STATUS_PENDING;
					break;
				}

				cur += 2;
				RedisBulk bulk = buildRedisBulk(mcount);
				reply->bulks->items[ix] = bulk;
				bulk->length = mcount;

				memcpy(bulk->content, (text + cur), mcount);
				cur += mcount;

				cur += 3;
			}
			break;
		}
	}

	return reply;
}

int get_bulk_size(char * text, int * cur,int length){
    char cnt[20] = {0};
    int scur = 0;

	if(*cur >= length){
		return -1;
	}

	int fake_size = 0;
	while(isdigit(text[*cur])){
		if(*cur == length -1){
			fake_size = 1;
		}
		cnt[scur++] = text[(*cur)++];
	}

	if(fake_size){
		return -1;
	}

	return atoi(cnt);
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