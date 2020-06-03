#include "dispatch.h"

#include "main.h"
#include "queue.h"

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