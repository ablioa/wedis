#include "debug.h"

#include "callbacks.h"

#include "resource.h"

TcpClient::TcpClient(Exception *excp):excep(excp){
	Buff = (char *)malloc(BUFF_SIZE*sizeof(char));
	bconnect = false;
}

TcpClient::~TcpClient(){
	WSACleanup();
	free(Buff);
}

TcpClient::init(){
	WORD SysVer=MAKEWORD(2,0);
	WSAStartup(SysVer,&wsa);

	socketid = socket(AF_INET,SOCK_STREAM,0);
	if(socketid == -1){
		return excep->DumpMessage(ERR_CANNOT_CREATESOCKET);
	}

	return 0;
}

int TcpClient::connect_to_server(HWND hwnd){
	int retVal;

	init();

	WSAAsyncSelect(socketid,hwnd,WM_USER+100,FD_CONNECT | FD_READ | FD_CLOSE | FD_WRITE);

	memset((void *)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.S_un.S_addr=inet_addr(REDIS_SERVER);
	serv_addr.sin_port=htons(REDIS_PORT);

	retVal = connect(
		socketid,
		(struct sockaddr*)&serv_addr,
		sizeof(struct sockaddr)
		);

	if( retVal == SOCKET_ERROR){
		retVal = WSAGetLastError();
		if(retVal != WSAEWOULDBLOCK){
			return excep->DumpMessage(ERR_CANNOT_CONNECTED);
		}
	}

	this->bconnect = true;	
	return 0;
}

void TcpClient::close_connect(){
	closesocket(socketid);
	socketid = 0;
	this->bconnect = false;
}

int TcpClient::senddata(char *buff,size_t size,int flags){
	int retVal;

	retVal = send(socketid,buff,size,flags);
	if(SOCKET_ERROR == retVal ){
		return excep->DumpMessage(ERR_SENDDATA_FAILED);
	}

	return retVal;
}

int TcpClient::receivedata(){
	int size;
	ZeroMemory(Buff,BUFF_SIZE);
	size = recv(socketid,Buff,BUFF_SIZE,0);
	Buff[size+1]=0x00;
	return size;
}

inline TcpClient::getskid(){
	return socketid;
}

char * TcpClient::getbuff(){
	return Buff;
}

inline bool TcpClient::getStatue(){
	return bconnect;
}

inline bool TcpClient::setStatue(bool st){
	bconnect = st; 
	return true;
}

eMsg pp[]={
	{0X0000,"",0},
	{0X0000,"TCP-Client have not connected to any Server!",MB_RETRYCANCEL},
	{0x0001,"Create Socket Failed!",MB_OK},
	{0x0002,"Send data failed!",MB_RETRYCANCEL},
	{0x0002,"TCP-Server have broken down!",MB_OK},
};

Exception::DumpMessage(long Err_Code){
	lpBoxParam->lpszText = err_list[Err_Code].Messag;
	lpBoxParam->dwStyle  = err_list[Err_Code].Style | MB_USERICON ;

	return MessageBoxIndirect(lpBoxParam);
}

Exception::Exception(eMsg *_err_list):err_list(_err_list){
	lpBoxParam = (MSGBOXPARAMS *)malloc(sizeof(MSGBOXPARAMS));
	ZeroMemory(lpBoxParam,sizeof(MSGBOXPARAMS));

	lpBoxParam->cbSize      = sizeof(MSGBOXPARAMS);
	lpBoxParam->hInstance   = GetModuleHandle(NULL);
	lpBoxParam->hwndOwner   = NULL;
	lpBoxParam->lpszCaption = "TCP_Client";
	lpBoxParam->lpszIcon    = MAKEINTRESOURCE(ID_MAIN);
	lpBoxParam->dwStyle     = MB_OKCANCEL;	
}

char * build_comment(const char * text,const char * pack){
	char * buff = (char *)malloc(sizeof(char) * 1024);
	memset(buff,0,sizeof(char) * 1024);
	sprintf(buff,"# ---- %s ----\r\n%s",text,pack);
	return buff;
}