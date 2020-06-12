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

    sprintf(msg,"taskType: %d",task->taskType);
    appendLog(msg);



    // switch(task->taskType){
    //     case 1:{
    //         break;
    //     }
    // }
}

void sendRedisCommand(char * command,char * dataKey,char * dataType,int type){
    Task * task = buildTask(type);

    task->dataKey = NULL;
    if(dataKey != NULL){
        task->dataKey = (char *) malloc(sizeof(char) * 256);
        memset(task->dataKey,0,sizeof(char ) * 256);
        sprintf(task->dataKey,"%s",dataKey);
    }

    if(dataType != NULL){
        task->dataType = (char *) malloc(sizeof(char) * 256);
        memset(task->dataType,0,sizeof(char ) * 256);
        sprintf(task->dataType,"%s",dataType);
    }

	char * encodedCmd = parse_command((char *)command,256);
    connection_senddata(mainModel->connection,encodedCmd,strlen(encodedCmd),0);
    
    addTask(pool,task);
}