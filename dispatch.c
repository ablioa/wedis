#include "dispatch.h"

#include "main.h"
#include "queue.h"

void sendRedisCommand(char * command,char * dataKey,int type){
    Task * task = buildTask(type);

    task->dataKey = NULL;
    if(dataKey != NULL){
        memset(mainModel->connection->key,0,256);
	    sprintf(mainModel->connection->key,"%s",dataKey);

        task->dataKey = dataKey;
    }

    mainModel->connection->cmdType = type;
	char * encodedCmd = parse_command((char *)command,256);
    connection_senddata(mainModel->connection,encodedCmd,strlen(encodedCmd),0);
    
    addTask(pool,task);
}