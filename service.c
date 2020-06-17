#include "service.h"

void redis_auth(char * password){
    char * cmd = (char *) malloc(sizeof(char) * 256);
	memset(cmd,0,sizeof(char) * 256);
	sprintf(cmd,"auth %s",password);

    RedisParam tmpe  = redis_build_param("auth");
    RedisParam param = redis_build_param(password);
    
    int total_size = param->s_length + 
    char * cmd = (char *) calloc(sizeof(char) * 256);
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

void redis_get_string(char * dataKey){
    char * cmd = (char *) calloc(256,sizeof(char));
	sprintf(cmd,"get %s",dataKey);
    
	sendRedisRequest(CMD_DATA,cmd,REDIS_STRING,dataKey);
}

void redis_get_list(char * dataKey){
	char * cmd = (char *) calloc(256,sizeof(char));
	sprintf(cmd,"lrange %s 0 -1",dataKey);

	sendRedisRequest(CMD_DATA,cmd, REDIS_LIST,dataKey);
}

void redis_get_hash(char * dataKey){
    char * cmd = (char *) calloc(256,sizeof(char));
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

void redis_delete_key(char * dataKey){
    char cmd[256];
    sprintf(cmd,"del %s",dataKey);

    sendRedisRequest(CMD_DELETE_KEY,cmd, REDIS_UNDEFINED,dataKey);
}

void redis_rename_key(char * dataKey,char * newKey){
    char cmd[256];
    sprintf(cmd,"rename %s %s",dataKey,newKey);
    sendRedisRequest(CMD_RENAME_KEY,cmd,REDIS_UNDEFINED,dataKey);
}

void redis_key_space(){
    sendRedisRequest(CMD_INFO_KEYSPACE,"info keyspace",REDIS_UNDEFINED,NULL);
}

void redis_info_stats(){
    sendRedisRequest(CMD_INFO_STATS,"info stats",REDIS_UNDEFINED,NULL);
}

void redis_serialize_params(RedisParams params){
    char head[128] = {0};
    sprintf(head,"%d%c%c",params->param_count,CHAR_CR, CHAR_LF);
    connection_senddata(mainModel->connection,head,strlen(head),0);

    for(int ix = 0; ix < params->param_count; ix ++){
        char * cmd = params->items[ix]->diagram;
        int    len = params->items[ix]->s_length;

        connection_senddata(mainModel->connection,cmd,len,0);
    }
}

void sendRedisRequest(CommandType cmdType,const DataType dataType,const char * dataKey){
    Task * task = buildTask(cmdType);

    task->dataType = dataType;
    task->dataKey = NULL;
    if(dataKey != NULL){
        task->dataKey = (char *) malloc(sizeof(char) * 256);
        memset(task->dataKey,0,sizeof(char ) * 256);
        sprintf(task->dataKey,"%s",dataKey);
    }

    addTask(pool,task);
}


// /**
//  * JUST FOR DEBUG
//  */

// #define KEY_SIZE 1024*1025*5
// #define VAL_SIZE 1024*1024*10
// #define CMD_SIZE 1024*1024*15+256
// void redis_add_big(){
//     char * key = (char*)calloc(2048,sizeof(char));
//     FILE * fkey = fopen("test/key.txt","r");
//     fread(key,1,2048,fkey);
//     fclose(fkey);

//     log_message(key);

//     char * value = (char*)calloc(1048576+1024,sizeof(char));
//     FILE * fval = fopen("test/value.txt","r");
//     fread(value,1,1048576+1024,fval);
//     fclose(fval);

//     log_message(value);

//     free(key);
//     free(value);

//     char * cmd = (char*)calloc(CMD_SIZE,sizeof(char));
//     sprintf(cmd,"set %s %s");
// }