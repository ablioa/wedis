#include "debug.h"

#include "callbacks.h"

#include "resource.h"

LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	Command * cmd = (Command *) GetWindowLong(hwnd,GWL_USERDATA);
	HWND hSubItem;
    switch (message){
        case WM_INITDIALOG:{
			//HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

			//HWND hCtrl2_0 = CreateWindowEx(0, WC_TABCONTROL, ("wewew"), 
			//	WS_VISIBLE | WS_CHILD, 0, 0, 489, 333, 
			//	hwnd, (HMENU)0, hinst, 0);
			//SendMessage(hCtrl2_0, WM_SETFONT, (WPARAM)0, FALSE);
			//
			//TCITEM tie;
			//LPSTR tabname[3]={"本地列表","网络收藏","网络电台"};
			//tie.mask=TCIF_TEXT;//|TCIF_IMAGE;
			////tie.iImage = -1; 
			//
			//for(INT i=0;i<3;i++){
			//	tie.pszText=tabname[i];
			//	TabCtrl_InsertItem(hCtrl2_0,i,&tie);
			//}


			//RECT rect;
			//GetClientRect(hCtrl2_0,&rect);
			//HWND ppp;
			//for(int i=0;i<3;i++){
			//	ppp=CreateDialog(hinst,MAKEINTRESOURCE(IDD_HARDWARE),hwnd,DLGPROC(SetBrkProc));
			//	MoveWindow(ppp,2,29,rect.right - rect.left-6,rect.bottom - rect.top-35,FALSE);
			//}

			//HWND ppp=CreateDialog(hinst,MAKEINTRESOURCE(IDD_HARDWARE),hwnd,DLGPROC(SetBrkProc));
 
			//ShowWindow(ppp,SW_SHOW);

			//////

			cmd = (Command*)malloc(sizeof(Command));
			memset(cmd,0,sizeof(Command));
			SetWindowLong(hwnd,GWL_USERDATA,(LONG)cmd);
			
			hSubItem = GetDlgItem(hwnd,IDC_MSGSEND);
			cmd->wndProc = (WNDPROC)SetWindowLong(hSubItem,GWL_WNDPROC,(LONG)SendBoxProc);
			SetWindowLong(hSubItem,GWL_USERDATA,(LONG)cmd);

			// 创建TCP链接
			Exception * exception1 = new Exception(pp);
			cmd->tcpClient = (TcpClient*) new TcpClient(exception1);

			cmd->tcpClient->connect_to_server(hwnd);
		}
        break;

        case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDM_CONNECT: 
					EnableWindow(GetDlgItem(hwnd,IDC_MSGSEND),TRUE);
					EnableWindow(GetDlgItem(hwnd,IDC_SEND),TRUE);
					//model->clt->connect_to_server(hwnd);
				break;

				case IDC_SEND:
					//MessageBox(hwnd,"发送数据","H",MB_OK);
					//view->SendData(*model);
				break;

				case IDM_DISCONNECT:
					EnableWindow(GetDlgItem(hwnd,IDC_MSGSEND),FALSE);
					EnableWindow(GetDlgItem(hwnd,IDC_SEND),FALSE);
					//model->clt->setStatue(false);
					//model->clt->close_connect();
					//view->UpdateView(*model);
					//EnableWindow(GetDlgItem(hwnd,IDC_MSGSEND),FALSE);
					//EnableWindow(GetDlgItem(hwnd,IDC_SEND),FALSE);
				break;

				//case IDM_EXIT:
				//	SendMessage(hwnd,WM_CLOSE,0,0);
				//break;
			}
        break;

		case WM_CLOSE:
			// 销毁TCP链接
			cmd->tcpClient->close_connect();
			EndDialog(hwnd,0);
		break;

		case WM_SOCKET:
			switch(LOWORD(lParam)){
				case FD_CONNECT:
					//if(lParam & 0xffff0000){
					//	model->excp->DumpMessage(ERR_CANNOT_CONNECTED);
					//}
					//else{
					//	model->clt->setStatue(true);
					//	view->UpdateView(*model);
					//	EnableWindow(GetDlgItem(hwnd,IDC_MSGSEND),TRUE);
					//	EnableWindow(GetDlgItem(hwnd,IDC_SEND),TRUE);
					//}
					MessageBox(hwnd,"connction created","title",MB_OK);
				break;
				
				case FD_READ:{
					char *buff;
					HWND hMsgDump;
					size_t curLeng;
					buff = cmd->tcpClient->getbuff();
					cmd->tcpClient->receivedata();
	
					hMsgDump = GetDlgItem(hwnd,IDC_OUTDUMP);
					curLeng = GetWindowTextLength(hMsgDump);
					SendMessage(hMsgDump,EM_SETSEL,curLeng,curLeng);
					SendMessage(hMsgDump,EM_REPLACESEL,FALSE,(LONG)buff);
				}
				break;

				case FD_WRITE:
					//MessageBox(hwnd,"connected","cap",NULL);
				break;

				case FD_CLOSE:
					MessageBox(hwnd,"connction closed","title",MB_OK);
					//	model->excp->DumpMessage(ERR_LINK_BREAKDOWN);
					//	model->clt->setStatue(false);
					///	view->UpdateView(*model);
					//	EnableWindow(GetDlgItem(hwnd,IDC_MSGSEND),FALSE);
					//	EnableWindow(GetDlgItem(hwnd,IDC_SEND),FALSE);
				break;
			}
		break;
    }

	return FALSE;
}

BOOL CALLBACK SendBoxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	Command * cmd = (Command *) GetWindowLong(hwnd,GWL_USERDATA);
	char buff[256];

	switch(msg){
		case WM_KEYUP:
			switch(wParam){
				case VK_UP:
					//model->cl->rollup();
					//view->UpdateData(*model);
				break;

				case VK_DOWN:
					//model->cl->rolldown();
					//view->UpdateData(*model);
				break;

				case VK_RETURN:
					GetWindowText(hwnd,buff,256);

					HWND parent = GetParent(hwnd);
					HWND hDump = GetDlgItem(parent,IDC_OUTDUMP);

					char * cmds = parse_command((char *)buff,256);
					char * comment = build_comment((char *)buff,cmds);
					int curLeng = GetWindowTextLength(hDump);
					SendMessage(hDump,EM_SETSEL,curLeng,curLeng);
					SendMessage(hDump,EM_REPLACESEL,FALSE,(LONG)comment);

					Command * pcmd = (Command *) GetWindowLong(hwnd,GWL_USERDATA);
					pcmd->tcpClient->senddata(cmds,strlen(cmds),0);

					SetWindowText(hwnd,NULL);

					free(cmds);
					free(comment);
				break;
			}
		break;

		case WM_RBUTTONDOWN:
		break;

		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case 900:
//					model->excp->DumpMessage(1);
				break;
			}
		break;

		default:{
		}
	}

	return CallWindowProc(cmd->wndProc,hwnd,msg,wParam,lParam);;
}

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

inline char * TcpClient::getbuff(){
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