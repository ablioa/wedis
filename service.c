#include "service.h"

RedisParams redis_build_params(int count){
    RedisParams params = (RedisParams)calloc(1,sizeof(struct redis_params));
    params->items = (RedisParam*)calloc(count,sizeof(struct redis_param));
}

void redis_add_param(RedisParams params,RedisParam param){
    params->items[params->param_count] = param;
    params->param_count++;
}

/**
 * 动态参数序列化
 * 2^32最大10位+2+2+1 = 15
 **/
RedisParam redis_build_param(char * content){
    RedisParam param = (RedisParam) calloc(1,sizeof(struct redis_param));

    param->content = content;
    param->length  = strlen(content);

    param->s_length = param->length + 15;
    param->diagram = (char*)calloc(param->s_length,sizeof(char));

    sprintf(param->diagram,"$%d%c%c%s%c%c",
        param->length,
        CHAR_CR, CHAR_LF,
        param->content,
        CHAR_CR, CHAR_LF);

    param->s_length = strlen(param->diagram);
    
    // log_message(param->diagram);

    return param;
}

void redis_serialize_params(RedisParams params){
    char head[128] = {0};
    sprintf(head,"*%d%c%c",params->param_count,CHAR_CR, CHAR_LF);
    connection_senddata(mainModel->connection,head,strlen(head),0);

    for(int ix = 0; ix < params->param_count; ix ++){
        char * cmd = params->items[ix]->diagram;
        int    len = params->items[ix]->s_length;
        connection_senddata(mainModel->connection,cmd,len,0);
    }
}

/**
 * auth password
 */
void redis_auth(char * password){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("auth"));
    redis_add_param(params,redis_build_param(password));

    redis_serialize_params(params);

    appendTask(CMD_AUTH,REDIS_UNDEFINED,NULL);
}

void redis_key_space(){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("info"));
    redis_add_param(params,redis_build_param("keyspace"));
    redis_serialize_params(params);

    appendTask(CMD_INFO_KEYSPACE,REDIS_UNDEFINED,NULL);
}

void redis_database_count(){
    RedisParams params = redis_build_params(3);
    redis_add_param(params,redis_build_param("config"));
    redis_add_param(params,redis_build_param("get"));
    redis_add_param(params,redis_build_param("databases"));
    redis_serialize_params(params);

    appendTask(CMD_DATABASE_COUNT,REDIS_UNDEFINED,NULL);
}

void redis_select(int database){
    char db[10] = {0};
    sprintf(db,"%d",database);

    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("select"));
    redis_add_param(params,redis_build_param(db));
    redis_serialize_params(params);

    appendTask(CMD_SELECT,REDIS_UNDEFINED,NULL);
}

/**
 * 查看数据库键值
 * TODO 需要优化为scan来处理
 */
void redis_keys(){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("keys"));
    redis_add_param(params,redis_build_param("*"));
    redis_serialize_params(params);

    appendTask(CMD_KEYS,REDIS_UNDEFINED,NULL);
}

void redis_data_type(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("type"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_serialize_params(params);

    appendTask(CMD_TYPE,REDIS_UNDEFINED,dataKey);
}

void redis_get_string(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("get"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_serialize_params(params);

    appendTask(CMD_DATA,REDIS_STRING,dataKey);
}

void redis_get_list(char * dataKey){
    RedisParams params = redis_build_params(4);
    redis_add_param(params,redis_build_param("lrange"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));
    redis_serialize_params(params);

    appendTask(CMD_DATA,REDIS_LIST,dataKey);
}

void redis_get_hash(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("hgetall"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_serialize_params(params);

    appendTask(CMD_DATA,REDIS_HASH,dataKey);
}

void redis_get_set(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("smembers"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_serialize_params(params);

    appendTask(CMD_DATA,REDIS_SET,dataKey);
}

void redis_get_zset(char * dataKey){
    RedisParams params = redis_build_params(5);
    redis_add_param(params,redis_build_param("zrange"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));
    redis_add_param(params,redis_build_param("withscores"));
    redis_serialize_params(params);
    
    appendTask(CMD_DATA,REDIS_ZSET,dataKey);
}

void redis_delete_key(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("del"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_serialize_params(params);

    appendTask(CMD_DELETE_KEY,REDIS_UNDEFINED,dataKey);
}

void redis_rename_key(char * dataKey,char * newKey){
    RedisParams params = redis_build_params(3);
    redis_add_param(params,redis_build_param("rename"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param(newKey));
    redis_serialize_params(params);

    appendTask(CMD_RENAME_KEY,REDIS_UNDEFINED,dataKey);
}

void redis_info_stats(){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("info"));
    redis_add_param(params,redis_build_param("stats"));
    redis_serialize_params(params);
    
    appendTask(CMD_INFO_STATS,REDIS_UNDEFINED,NULL);
}

void appendTask(CommandType cmdType,const DataType dataType,const char * dataKey){
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



/**
 * JUST FOR DEBUG
 */

#define KEY_SIZE 1024*1025*5
#define VAL_SIZE 1024*1024*10
#define CMD_SIZE 1024*1024*15+256
void redis_add_big(){
    char * key = (char*)calloc(2048,sizeof(char));
    FILE * fkey = fopen("../test/key.txt","r");
    fread(key,1,2048,fkey);
    fclose(fkey);

    log_message(key);

    char * value = (char*)calloc(1048576+1024,sizeof(char));
    FILE * fval = fopen("../test/value.txt","r");
    fread(value,1,1048576+1024,fval);
    fclose(fval);

    log_message(value);

    RedisParams params = redis_build_params(3);
    redis_add_param(params,redis_build_param("set"));
    redis_add_param(params,redis_build_param(key));
    redis_add_param(params,redis_build_param(value));
    redis_serialize_params(params);
    
    appendTask(CMD_SET_STRING,REDIS_UNDEFINED,key);

    //free(key);
    //free(value);

    //char * cmd = (char*)calloc(CMD_SIZE,sizeof(char));
   // sprintf(cmd,"set %s %s");
}