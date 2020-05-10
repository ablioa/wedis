#ifndef debug_h
#define debug_h

#include <windows.h>
#include <winsock.h>

#include <commctrl.h>

#include "redis.h"

#define BUFF_SIZE 4096

#define REDIS_SERVER "192.168.1.9"
#define REDIS_PORT   6379


#define ERR_NORMAL					0x0000
#define ERR_CANNOT_CONNECTED		0X0001
#define ERR_CANNOT_CREATESOCKET		0x0002
#define ERR_SENDDATA_FAILED			0x0003
#define ERR_LINK_BREAKDOWN			0x0004

#define WM_SOCKET WM_USER+100


typedef struct err_info{
	LONG ecode;
	char Messag[MAX_PATH];
	UINT Style;
}eMsg;


extern eMsg pp[];

class Exception{
	eMsg *err_list;

	MSGBOXPARAMS *lpBoxParam;

public:
	Exception(eMsg *_err_list);

	DumpMessage(long Err_Code);
};

class TcpClient{
    private:
    	char *Buff;

		int socketid;

		struct sockaddr_in serv_addr;

		WSADATA wsa;

    	bool bconnect;

    public:
		Exception *excep;

    	TcpClient(Exception *excep);

    	~TcpClient();

    	int init();

    	int connect_to_server(HWND hwnd);

    	void close_connect();

    	int senddata(char *buff,size_t size,int flags);

    	int receivedata();

		int getskid();

		char * getbuff();

		bool getStatue();

		bool setStatue(bool st);

};

char * build_comment(const char * text,const char * pack);

#endif
