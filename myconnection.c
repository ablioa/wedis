#include "myconnection.h"

void ginit(){
    WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0){
		exit(0);
	}
}

RedisConnection init(char * address,int port){
    RedisConnection stream = (RedisConnection)calloc(1,sizeof(struct redis_connection));

    stream->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (stream->socket == INVALID_SOCKET){
		printf("invalid socket !");
		return NULL;
	}

	stream->address.sin_family = AF_INET;
	stream->address.sin_port = htons(port);
	stream->address.sin_addr.S_un.S_addr = inet_addr(address);
	if (connect(stream->socket, (struct sockaddr *)&(stream->address), sizeof(stream->address)) == SOCKET_ERROR){
		MessageBox(NULL,"connect error !","OK",MB_OK);
		closesocket(stream->socket);
		return NULL;
	}

	return stream;
}

void sendmsg(RedisConnection stream,char * message){
    send(stream->socket, message, strlen(message), 0);
    
    //char recData[20];
    //int ret = recv(stream->socket, recData, 20, 0);
    //if (ret > 0){
    //	recData[ret] = 0x00;
    //	printf("%s\n",recData);
    //}
}

RedisReply receive_msg(RedisConnection stream){
	char recData[1024 * 1024] = {0};
	int cur = 0;

	RedisReply reply = NULL;
    int ret = recv(stream->socket, recData+cur, 1024 * 1024, 0);	
	if(ret > 0){
		//cur+=ret;
		reply = read_replay(recData,ret);
		//ret = recv(stream->socket, recData+cur, 255, 0);
	}
	
	return reply;
}

void close(RedisConnection stream){
	closesocket(stream->socket);
	WSACleanup();
}