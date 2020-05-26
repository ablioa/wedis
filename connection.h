#ifndef connection_h
#define connection_h

#include <windows.h>
#include <winsock.h>

#include "redis.h"
#include "exception.h"

#define BUFF_SIZE 4096

//define REDIS_SERVER "139.224.207.71"
#define REDIS_SERVER "192.168.1.9"
#define REDIS_PORT   6379

#define WM_SOCKET WM_USER+100

#define WM_DT_STRING WM_USER+200
#define WM_DT_LIST   WM_USER+200
#define WM_DT_HASH   WM_USER+200

#define PT_SELECT 0
#define PT_KEYS   1
#define PT_TYPE   2
#define PT_DATA   3

typedef struct {
     WSADATA wsa;

    struct sockaddr_in serv_addr;

    int socketid;

    BOOL bconnect;

    /** 数据交互缓冲区 */
    char * buff;

    /** 当前业务类型 */
    int cmdType;

    char key[256];
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