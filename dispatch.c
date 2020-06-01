#include "dispatch.h"

#include "main.h"
#include "queue.h"

void sendRedisCommand(char * command,char * dataKey,int type){
	memset(mainModel->connection->key,0,256);
	sprintf(mainModel->connection->key,"%s",dataKey);
    
    mainModel->connection->cmdType = type;
	char * encodedCmd = parse_command((char *)command,256);
    connection_senddata(mainModel->connection,encodedCmd,strlen(encodedCmd),0);

    TaskPool * task = buildTask(type);
    addTask(pool,task);
}