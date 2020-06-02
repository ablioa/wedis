#include "connection.h"

TcpConnection * build_connection(char * host,int port,char * password){
    TcpConnection * connection = (TcpConnection *)malloc(sizeof(TcpConnection));
    ZeroMemory(connection,sizeof(TcpConnection));

	strcpy(connection->host,host);
	strcpy(connection->password,password);
	connection->port = port;

    return connection;
}

int connect_to_server(TcpConnection * connection,HWND hwnd){
    int retVal;

    connection->buff = (char *)malloc(BUFF_SIZE*sizeof(char));
	connection->bconnect = FALSE;

	WORD SysVer=MAKEWORD(2,0);
	WSAStartup(SysVer,&(connection->wsa));

	connection->socketid = socket(AF_INET,SOCK_STREAM,0);
	if(connection->socketid == -1){
		DumpMessage(ERR_CANNOT_CREATESOCKET);
	}

	WSAAsyncSelect(connection->socketid,hwnd,WM_USER+100,FD_CONNECT | FD_READ | FD_CLOSE | FD_WRITE);

	memset((void *)&(connection->serv_addr),0,sizeof(struct sockaddr_in));
	connection->serv_addr.sin_family = AF_INET;
	connection->serv_addr.sin_addr.S_un.S_addr=inet_addr(connection->host);
	connection->serv_addr.sin_port=htons(connection->port);

	retVal = connect(
		connection->socketid,
		(struct sockaddr*)&(connection->serv_addr),
		sizeof(struct sockaddr)
		);

	if( retVal == SOCKET_ERROR){
		retVal = WSAGetLastError();
		if(retVal != WSAEWOULDBLOCK){
			DumpMessage(ERR_CANNOT_CONNECTED);
		}
	}

	connection->bconnect = TRUE;	
	return 0;
}

int connection_close_connect(TcpConnection * connection){
	closesocket(connection->socketid);
	connection->socketid = 0;
	connection->bconnect = FALSE;

    WSACleanup();
    free(connection->buff);

    return 0;
}

int connection_senddata(TcpConnection * connection,char *buff,size_t size,int flags){
    int retVal;

	retVal = send(connection->socketid,buff,size,flags);
	if(SOCKET_ERROR == retVal ){
		DumpMessage(ERR_SENDDATA_FAILED);
	}

	return retVal;
}

int connection_receivedata(TcpConnection * connection){
	int size;
	ZeroMemory(connection->buff,BUFF_SIZE);
	size = recv(connection->socketid,connection->buff,BUFF_SIZE,0);
	connection->buff[size+1]=0x00;
	return size;
}

int connection_get_sockid(TcpConnection * connection){
    return connection->socketid;
}

char * connection_get_buffer(TcpConnection * connection){
    return connection->buff;
}

BOOL connection_get_status(TcpConnection * connection){
    return connection->bconnect;
}

BOOL connection_set_status(TcpConnection * connection,BOOL status){
    connection->bconnect = status;
    return connection->bconnect;
}