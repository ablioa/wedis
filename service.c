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
    
    return param;
}

/**
 * 发送报文 
 */
RedisReply redis_serialize_params(RedisConnection stream,RedisParams params){
    char head[128] = {0};
    sprintf(head,"*%d%c%c",params->param_count,CHAR_CR, CHAR_LF);
    sendmsg(stream,head);

    for(int ix = 0; ix < params->param_count; ix ++){
        char * cmd = params->items[ix]->diagram;
        int    len = params->items[ix]->s_length;
        sendmsg(stream,cmd);
    }

    return receive_msg(stream);
}

/**
 * auth password
 */
RedisParams redis_auth(char * password){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("auth"));
    redis_add_param(params,redis_build_param(password));

    return params;
}

RedisParams redis_key_space(){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("info"));
    redis_add_param(params,redis_build_param("keyspace"));

    return params;
}

RedisParams redis_database_count(){
    RedisParams params = redis_build_params(3);
    redis_add_param(params,redis_build_param("config"));
    redis_add_param(params,redis_build_param("get"));
    redis_add_param(params,redis_build_param("databases"));

    return params;
}

RedisParams redis_select(int database){
    char db[10] = {0};
    sprintf(db,"%d",database);

    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("select"));
    redis_add_param(params,redis_build_param(db));

    return params;
}

/**
 * 查看数据库键值
 * TODO 需要优化为scan来处理
 */
RedisParams redis_keys(){
    // RedisParams params = redis_build_params(2);
    // redis_add_param(params,redis_build_param("keys"));
    // redis_add_param(params,redis_build_param("*"));

    RedisParams params = redis_build_params(6);
    redis_add_param(params,redis_build_param("scan"));
    redis_add_param(params,redis_build_param("1"));
    redis_add_param(params,redis_build_param("match"));
    redis_add_param(params,redis_build_param("*"));
    redis_add_param(params,redis_build_param("count"));
    redis_add_param(params,redis_build_param("20"));

    return params;
}

RedisParams redis_data_type(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("type"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_get_string(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("get"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_get_list(char * dataKey){
    RedisParams params = redis_build_params(4);
    redis_add_param(params,redis_build_param("lrange"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));

    return params;
}

RedisParams redis_get_hash(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("hgetall"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_get_set(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("smembers"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_get_zset(char * dataKey){
    RedisParams params = redis_build_params(5);
    redis_add_param(params,redis_build_param("zrange"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param("0"));
    redis_add_param(params,redis_build_param("-1"));
    redis_add_param(params,redis_build_param("withscores"));

    return params;
}

RedisParams redis_delete_key(char * dataKey){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("del"));
    redis_add_param(params,redis_build_param(dataKey));

    return params;
}

RedisParams redis_rename_key(char * dataKey,char * newKey){
    RedisParams params = redis_build_params(3);
    redis_add_param(params,redis_build_param("rename"));
    redis_add_param(params,redis_build_param(dataKey));
    redis_add_param(params,redis_build_param(newKey));

    return params;
}

RedisParams redis_info_stats(){
    RedisParams params = redis_build_params(2);
    redis_add_param(params,redis_build_param("info"));
    redis_add_param(params,redis_build_param("stats"));

    return params;
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
