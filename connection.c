#include "connection.h"

void ginit(){
    WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0){
		exit(0);
	}
}

RedisConnection init(char * address,int port){
    RedisConnection stream = (RedisConnection)calloc(1,sizeof(struct redis_connection));

	stream->host = (char*)calloc(255,sizeof(char));
	strcpy(stream->host,address);
	stream->port = port;

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
}

// RedisReply receive_msg(RedisConnection stream){
// 	char localbuff[4096] = {0};

// 	int ret  = 0;
// 	RedisReply reply = NULL;

// 	stream->capacity  = 4096;
// 	stream->free_size = stream->capacity;
// 	stream->read_buff = (char*) calloc(stream->capacity,sizeof(char));
	
// 	do{
// 		ret = recv(stream->socket, localbuff, 4096, 0);
// 		if(stream->free_size <= ret){
// 			stream->capacity  += 4096;
// 			stream->free_size += 4096;
// 			stream->read_buff = (char*) realloc(stream->read_buff,stream->capacity);
// 		}

// 		memcpy(stream->read_buff + ((stream->capacity - stream->free_size)),localbuff,ret);
// 		stream->free_size -= ret;
// 		reply = read_reply(stream->read_buff,(stream->capacity - stream->free_size));
// 	}while(reply->reply_status != REPLY_STATUS_DONE);

// 	free(stream->read_buff);

// 	return reply;
// }

void close(RedisConnection stream){
	closesocket(stream->socket);
	WSACleanup();
}