#ifndef connection_h
#define connection_h

#include <windows.h>
#include <winsock.h>
#include "redis.h"
#include "exception.h"

#define BUFF_SIZE 4096

//define REDIS_SERVER "192.168.1.9"
//define REDIS_PORT   6379

#define REDIS_SERVER "139.224.207.71"
#define REDIS_PORT   6379

#define WM_SOCKET WM_USER+100

typedef struct {
    char * Buff;

    int socketid;

    struct sockaddr_in serv_addr;

    WSADATA wsa;

    BOOL bconnect;

}TcpConnection;

TcpConnection * build_connection();

char * build_comment(const char * text,const char * pack);

int connect_to_server(TcpConnection * connection,HWND hwnd);

int connection_close_connect(TcpConnection * connection);

int connection_senddata(TcpConnection * connection,char *buff,size_t size,int flags);

int connection_receivedata(TcpConnection * connection);

int connection_get_sockid(TcpConnection * connection);

char * connection_get_buffer(TcpConnection * connection);

BOOL connection_get_status(TcpConnection * connection);

BOOL connection_set_status(TcpConnection * connection,BOOL status);

#endif