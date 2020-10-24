#ifndef connection_h
#define connection_h

#include <windows.h>
#include <winsock.h>

#include "redis.h"
#include "exception.h"

#define BUFF_SIZE 1024*1024*5

#define WM_SOCKET WM_USER+100

#define WM_DT_STRING WM_USER+200
#define WM_DT_LIST   WM_USER+200
#define WM_DT_HASH   WM_USER+200

typedef struct {
    WSADATA wsa;

    struct sockaddr_in serv_addr;

    int socketid;

    BOOL bconnect;

    /** 数据交互缓冲区 */
    char * buff;

    int port;

    char host[128];

    char password[128];

    HTREEITEM hostHandle;

    HWND connectionHolder;
}TcpConnection;

TcpConnection * build_connection(char * host,int port,char * password);

char * build_comment(const char * text,const char * pack);

int connect_to_server(TcpConnection * connection,HWND hwnd);

int connection_close_connect(TcpConnection * connection);

int connection_senddata(TcpConnection * connection,char *buff,size_t size,int flags);

int connection_receivedata(TcpConnection * connection);

int connection_get_sockid(TcpConnection * connection);

char * connection_get_buffer(TcpConnection * connection);

BOOL connection_get_status(TcpConnection * connection);

BOOL connection_set_status(TcpConnection * connection,BOOL status);

LRESULT CALLBACK redisClientWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif