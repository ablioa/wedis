#include "test.h"

void buildConnection(HWND hwnd,int cmd){
    Host * host = getHostByIndex(appConfig,cmd);
	if(host == NULL){
		return;
	}

	// TODO 判定连接创建结果
	mainModel->connection =  build_connection(host->host,host->port,host->password);
    connect_to_server(mainModel->connection,hwnd);

	// TODO 封装一下
	char connName[255] = {0};
	sprintf(connName,"%s:%d",host->host,host->port);
	HTREEITEM parentHandle = addHostNode(connName);
	mainModel->connection->hostHandle = parentHandle;
}

void auth(){

}

void wedis_test(){
    buildConnection(mainModel->mainWindowHwnd,IDM_CUSTOMER_HOST);
    redis_auth(mainModel->connection->password);

    // 连接
    // 登录
    // 系统信息
    // 查看列表
}