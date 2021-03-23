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

connect:
    if (connect(stream->socket, (struct sockaddr *)&(stream->address), sizeof(stream->address)) == SOCKET_ERROR){
        int result = DumpMessage(ERR_CANNOT_CONNECTED);
        if(result == 4){
            goto connect;
        }else{
            closesocket(stream->socket);
        }
        return NULL;
    }

    return stream;
}

void sendmsg(RedisConnection stream,char * message,int length){
    send(stream->socket, message, length, 0);
}

void close(RedisConnection stream){
    closesocket(stream->socket);
    WSACleanup();
}
