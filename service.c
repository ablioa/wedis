#include "service.h"

void redis_auth(char * password){
    char * cmd = (char *) malloc(sizeof(char) * 256);
	memset(cmd,0,sizeof(char) * 256);
	sprintf(cmd,"auth %s",password);

    sendRedisRequest(CMD_AUTH,cmd,REDIS_UNDEFINED,NULL);
}

void redis_database_count(){
    sendRedisRequest(CMD_DATABASE_COUNT,"config get databases",REDIS_UNDEFINED,NULL);
}

void redis_select(int database){
    char * cmd = (char*)malloc(sizeof(char)*128);
    memset(cmd,0,sizeof(char) * 128);
    sprintf(cmd,"select %d",database);

    sendRedisRequest(CMD_SELECT,cmd,REDIS_UNDEFINED,NULL);
}

/**
 * 查看数据库键值
 * TODO 需要优化为scan来处理
 */
void redis_keys(){
    char cmd[256] = {0};
    memset(cmd,0,sizeof(char) * 256);
    sprintf(cmd,"keys *");

    sendRedisRequest(CMD_KEYS,cmd,REDIS_UNDEFINED,NULL);
}

void redis_data_type(char * dataKey){
    char * cmd = (char*)malloc(sizeof(char)*128);
	memset(cmd,0,sizeof(char) * 128);
	sprintf(cmd,"type %s",dataKey);

    sendRedisRequest(CMD_TYPE,cmd,REDIS_UNDEFINED,dataKey);
}

void redis_get_string(char * key){
    const char * cmd = (char *) malloc(sizeof(char) * 256);
	memset(cmd,0,sizeof(char) * 256);
	sprintf(cmd,"get %s",key);
    
	sendRedisRequest(CMD_DATA,cmd,REDIS_STRING,key);
}

void redis_get_list(char * dataKey){
	char * cmd = (char *) malloc(sizeof(char) * 256);
	memset(cmd,0,sizeof(char) * 256);
	sprintf(cmd,"lrange %s 0 -1",dataKey);

	sendRedisRequest(CMD_DATA,cmd, REDIS_LIST,dataKey);
}

void redis_get_hash(char * dataKey){
    char * cmd = (char *) malloc(sizeof(char) * 256);
	memset(cmd,0,sizeof(char) * 256);
	sprintf(cmd,"hgetall %s",dataKey);

    sendRedisRequest(CMD_DATA,cmd, REDIS_HASH,dataKey);
}

void redis_get_set(char * dataKey){
    char * cmd = (char *) malloc(sizeof(char) * 256);
	memset(cmd,0,sizeof(char) * 256);
	sprintf(cmd,"smembers %s",dataKey);

    sendRedisRequest(CMD_DATA,cmd, REDIS_SET,dataKey);
}

void redis_get_zset(char * dataKey){
    char * cmd = (char *) malloc(sizeof(char) * 256);
	memset(cmd,0,sizeof(char) * 256);
	sprintf(cmd,"zrange %s 0 -1 withscores",dataKey);
    
    sendRedisRequest(CMD_DATA,cmd, REDIS_ZSET,dataKey);
}

void sendRedisRequest(CommandType cmdType,const char * cmd,const DataType dataType,const char * dataKey){
    Task * task = buildTask(cmdType);

    task->dataType = dataType;
    task->dataKey = NULL;
    if(dataKey != NULL){
        task->dataKey = (char *) malloc(sizeof(char) * 256);
        memset(task->dataKey,0,sizeof(char ) * 256);
        sprintf(task->dataKey,"%s",dataKey);
    }
   

	char * encodedCmd = parse_command((char *)cmd,256);
    connection_senddata(mainModel->connection,encodedCmd,strlen(encodedCmd),0);

    addTask(pool,task);
}
