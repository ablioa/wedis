#include "dispatch.h"

void dispatch(Task * task,RedisReply * data){
    char msg[255] = {0};

    if(task == NULL){
        appendLog("EMPTY TASK");
        return;
    }

    if(data == NULL){
        appendLog("EMPTY DATA");
        return;
    }

    // TODO 这里要校验返回数据的类型，保证没有出错，能够正常处理

    sprintf(msg,"taskType: %d",task->taskType);
    appendLog(msg);

    switch(task->taskType){
        case CMD_AUTH:{
            if(data->type == REPLY_ERROR){
                MessageBox(NULL,data->error,"title",MB_OK);
            }else if(data->type == REPLY_STATUS){
                if(strcmp("OK",data->status) == 0){
                    //redis_database_count();
                    redis_key_space();
                }
            }
            break;
        }

        case CMD_DATABASE_COUNT:{
            int dbCount = atoi(data->bulks[1]);
            addDatabaseNode(dbCount);
            break;
        }

        case CMD_SELECT:{
            if(data->type != REPLY_STATUS || strcmp("OK",data->status) != 0){
                log_message("database select error");
            }
            break;
        }

        case CMD_KEYS:{
            if(data->type == REPLY_MULTI){
                addDataNode(data);
            }else{
                log_message("error data on KEYS request");
            }
            break;
        }

        case CMD_TYPE:{
            if(data->type == REPLY_STATUS){
                DataType dataType = checkDataType(data->status);
                if(dataType == REDIS_UNDEFINED){
                    log_message("undefined data type");
                    break;
                }

                handleDataType(task,dataType);        
            }else{
                log_message("data type check error");
            }
        
            break;
        }

        case CMD_DATA:{
            if(data->type == REPLY_ERROR){
                log_message("data fetch error!");
                break;
            }

            handleRedisData(task,data);
            break;
        }

        case CMD_DELETE_KEY:{
            break;
        }

        case CMD_INFO_KEYSPACE:{
            if(data->type == REPLY_BULK){
                Keyspace space = parseKeyspace(data->bulk);
                handleKeyspace(space);
            }
            break;
        }
    }
    
}
