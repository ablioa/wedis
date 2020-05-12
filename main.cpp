#include "main.h"


HINSTANCE hInstance;
TCHAR     szFrameClass[] = TEXT ("MdiFrame") ;

#define WOWOWO "wowowo"
#define CONSOLE_WINDOW "CONSOLE_WINDOW"

BOOL CALLBACK exportBtnProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	DataView * dataView = (DataView *)GetWindowLong(hwnd,GWL_USERDATA);
	
	switch(msg){
		case WM_LBUTTONUP:{
//			char * fname = mGetOpenFileName(hwnd);
			// TODO 导出结构
			break;
		}
	}

	return CallWindowProc(dataView->exportBtnProc,hwnd,msg,wParam,lParam);;
}

BOOL CALLBACK ttlBtnProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	DataView * dataView = (DataView *)GetWindowLong(hwnd,GWL_USERDATA);
	switch(msg){
		case WM_LBUTTONUP:{
			HINSTANCE hInst= (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
			DialogBox (hInst,MAKEINTRESOURCE (IDD_GOTOLINE),hwnd,GoToLineDlgProc);
			break;
		}
	}
	return CallWindowProc(dataView->ttlBtnProc,hwnd,msg,wParam,lParam);;
}

BOOL CALLBACK CmdBoxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	ConsoleView * cmd = (ConsoleView *) GetWindowLong(hwnd,GWL_USERDATA);
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
					//HWND parent = GetParent(hwnd);

					char * cmds = parse_command((char *)buff,256);
					char * comment = build_comment((char *)buff,cmds);
					int curLeng = GetWindowTextLength(cmd->logHwnd);
					SendMessage(cmd->logHwnd,EM_SETSEL,curLeng,curLeng);
					SendMessage(cmd->logHwnd,EM_REPLACESEL,FALSE,(LONG)comment);

					cmd->mainModel->tcpClient->senddata(cmds,strlen(cmds),0);
					//
					//Command * pcmd = (Command *) GetWindowLong(hwnd,GWL_USERDATA);
					//cmd->mainModel->tcpClient->senddata(cmds,strlen(cmds),0);
					//
					SetWindowText(hwnd,NULL);
					//
					free(cmds);
					free(comment);
				break;
			}
		break;
	}

	return CallWindowProc(cmd->defaultCmdProc,hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK consoleWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	HINSTANCE hinst = hInstance;
	ConsoleView * consoleView = (ConsoleView *)GetWindowLong(hwnd,GWL_USERDATA);

	RECT rect;

    switch(msg){
		case WM_CREATE:{
			consoleView = (ConsoleView*)malloc(sizeof(ConsoleView));
			memset(consoleView,0,sizeof(ConsoleView));
			
			HWND logHwnd  = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | WS_VSCROLL  |ES_MULTILINE, 0, 0, 100, 100, hwnd, (HMENU)0, hinst, 0);
            HWND cmdHwnd  = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER , 0, 100, 100, 24, hwnd, (HMENU)0, hinst, 0);
            
			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(logHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(cmdHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);

			consoleView->logHwnd = logHwnd;
			consoleView->cmdHwnd = cmdHwnd;
			SetWindowLong(hwnd,GWL_USERDATA,(LONG)consoleView);

			consoleView->defaultCmdProc = (WNDPROC)SetWindowLong(cmdHwnd,GWL_WNDPROC,(LONG)CmdBoxProc);
			SetWindowLong(cmdHwnd,GWL_USERDATA,(LONG)consoleView);

			HWND hParent = GetParent(hwnd);
			MainModel * mainModel = (MainModel *)GetWindowLong(hParent,GWL_USERDATA);
			mainModel->logHwnd = logHwnd;

			consoleView->mainModel = mainModel;
			break;
		}

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(consoleView->logHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top-24,true);
			MoveWindow(consoleView->cmdHwnd,0,rect.bottom-rect.top-24,rect.right-rect.left,24,true);
			break;
		}

		case WM_LBUTTONDOWN:{
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK wowowProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	HINSTANCE hinst = hInstance;
	DataView * dataView = (DataView *)GetWindowLong(dataHwnd,GWL_USERDATA);

	RECT rect;

    switch(msg){
		case WM_CREATE:{
            HWND keyEditHwnd  = CreateWindowEx(0, WC_EDIT, ("11"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 5, 5, 240, 24, dataHwnd, (HMENU)0, hinst, 0);    
            HWND renameBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("Rename"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 250, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);     
            HWND ttlBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("TTL"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 315, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);
            HWND removeBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("Remove"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 380, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);
            HWND reloadBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Reload"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 445, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);
            HWND viewTypeHwnd  = CreateWindowEx(0, WC_COMBOBOX, (""), WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_HASSTRINGS, 510, 5, 120, 24, dataHwnd, (HMENU)0, hinst, 0);
            
            HWND dataViewHwnd  = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 5, 34, 625, 240, dataHwnd, (HMENU)0, hinst, 0);
            
            HWND saveBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("Save"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 570, 279, 60, 24, dataHwnd, (HMENU)0, hinst, 0);     
            HWND exportBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Export"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 505, 279, 60, 24, dataHwnd, (HMENU)0, hinst, 0); 
            
            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(dataViewHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(saveBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(exportBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(keyEditHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(renameBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(viewTypeHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(ttlBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(removeBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(reloadBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);

			dataView = (DataView*)malloc(sizeof(DataView));
			memset(dataView,0,sizeof(DataView));

			dataView->dataViewHwnd  = dataViewHwnd;
			dataView->exportBtnHwnd = exportBtnHwnd;
			dataView->saveBtnHwnd   = saveBtnHwnd;
			dataView->viewTypeHwnd  =viewTypeHwnd;

			dataView->reloadBtnHwnd = reloadBtnHwnd;
			dataView->removeBtnHwnd = removeBtnHwnd;
			dataView->renameBtnHwnd = renameBtnHwnd;
			dataView->ttlBtnHwnd    = ttlBtnHwnd;
			dataView->keyEditHwnd   = keyEditHwnd;

			SetWindowLong(dataHwnd,GWL_USERDATA,(LONG)dataView);

			dataView->ttlBtnProc = (WNDPROC)SetWindowLong(ttlBtnHwnd,GWL_WNDPROC,(LONG)ttlBtnProc);
			SetWindowLong(ttlBtnHwnd,GWL_USERDATA,(LONG)dataView);

			dataView->exportBtnProc = (WNDPROC)SetWindowLong(exportBtnHwnd,GWL_WNDPROC,(LONG)exportBtnProc);
			SetWindowLong(exportBtnHwnd,GWL_USERDATA,(LONG)dataView);

		    break;
		}

        case WM_LBUTTONDOWN:{
			break;
        }

		case WM_SIZE:{
			GetClientRect(dataHwnd,&rect);

			MoveWindow(dataView->dataViewHwnd,5,34,rect.right-rect.left-5-5,rect.bottom-rect.top-68-5,true);
			MoveWindow(dataView->exportBtnHwnd,rect.right-rect.left-60-5,rect.bottom-rect.top-24-5,60,24,true);
			MoveWindow(dataView->saveBtnHwnd,rect.right-rect.left-125-5,rect.bottom-rect.top-24-5,60,24,true);
			MoveWindow(dataView->viewTypeHwnd,rect.right-rect.left-120-5,5,120,24,true);
			MoveWindow(dataView->reloadBtnHwnd,rect.right-rect.left-5-120-5-60,5,60,24,true);
			MoveWindow(dataView->removeBtnHwnd,rect.right-rect.left-5-120-5-60-5-60,5,60,24,true);
			MoveWindow(dataView->ttlBtnHwnd,rect.right-rect.left-5-120-5-60-5-60-5-60,5,60,24,true);
			MoveWindow(dataView->renameBtnHwnd,rect.right-rect.left-5-120-5-60-5-60-5-60-5-60,5,60,24,true);
			MoveWindow(dataView->keyEditHwnd,5,5,rect.right-rect.left-5-120-5-60-5-60-5-60-5-60-5-5,24,true);

			break;
		}
	}

	return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

void initpan(){
    WNDCLASSEX hSplitClass;
    hSplitClass.cbSize        = sizeof(hSplitClass);
    hSplitClass.style         = 0;
    hSplitClass.lpfnWndProc   = wowowProc;
    hSplitClass.cbClsExtra    = 0;
    hSplitClass.cbWndExtra    = 0;
    hSplitClass.hInstance     = hInstance;
    hSplitClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    hSplitClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    hSplitClass.hbrBackground = (HBRUSH) GetStockObject (COLOR_BTNFACE + 1);//CreateSolidBrush(RGB(255,0,0));
    hSplitClass.lpszMenuName  = 0;
    hSplitClass.lpszClassName = WOWOWO;
    hSplitClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&hSplitClass);

    WNDCLASSEX consoleWinClass;
    consoleWinClass.cbSize        = sizeof(consoleWinClass);
    consoleWinClass.style         = 0;
    consoleWinClass.lpfnWndProc   = consoleWindowProc;
    consoleWinClass.cbClsExtra    = 0;
    consoleWinClass.cbWndExtra    = 0;
    consoleWinClass.hInstance     = hInstance;
    consoleWinClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    consoleWinClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    consoleWinClass.hbrBackground = (HBRUSH) GetStockObject (COLOR_BTNFACE + 1);
    consoleWinClass.lpszMenuName  = 0;
    consoleWinClass.lpszClassName = CONSOLE_WINDOW;
    consoleWinClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&consoleWinClass);
}

BOOL initApplication(){
	HANDLE hmod = LoadLibrary("srcview.dll");
    if (hmod==NULL){
    	MessageBox(NULL,"The Scintilla DLL could not be loaded.","Error loading Scintilla",MB_OK | MB_ICONERROR);
		return FALSE;
    }

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
    if(!initApplication()){
	    return 0;
	}

    save_all();

	hInstance = hInst;

	initpan();

	WNDCLASSEX  mainClass;
	mainClass.cbSize = sizeof (WNDCLASSEX);
	mainClass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	mainClass.lpfnWndProc=MainWndProc;
	mainClass.lpszClassName = szFrameClass;
	mainClass.hInstance = hInst;
	mainClass.hIcon = 0;
	mainClass.hIconSm = 0;
	mainClass.lpszMenuName = MAKEINTRESOURCE (ID_MAIN);
	mainClass.cbClsExtra = 0;
	mainClass.cbWndExtra = 0;
	mainClass.hbrBackground = (HBRUSH) GetStockObject (COLOR_BTNFACE + 1);
	mainClass.hCursor = LoadCursor (0, IDC_ARROW);

    if (!RegisterClassEx (&mainClass)){
         MessageBox (NULL, TEXT ("This program requires Windows NT!"),
                     TEXT("appName"), MB_ICONERROR) ;
         return 0 ;
    }

	HWND hwndFrame = CreateWindowEx(WS_EX_LEFT,szFrameClass, TEXT ("MDI Demonstration"),
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
							   CW_USEDEFAULT, 0,
							   400, 300,
                               NULL,NULL, hInst, NULL) ;

    ShowWindow (hwndFrame, cmdShow) ;
    UpdateWindow (hwndFrame) ;

	HANDLE hAccel = LoadAccelerators(hInst,MAKEINTRESOURCE(IDA_RUN));

	MSG  msg;
	while (GetMessage (&msg, 0, 0, 0)){
		if(!TranslateAccelerator(msg.hwnd,(HACCEL)hAccel,&msg)){
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
    }

    return msg.wParam;
}



LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    //Controller * pCtrl = (Controller *)GetWindowLong(hwnd,GWL_USERDATA);
	MainModel * mainModel = (MainModel *)GetWindowLong(hwnd,GWL_USERDATA);
//	RECT rt;
//	POINT pos;
    switch (message){
		case WM_CREATE:{
			mainModel = (MainModel * )malloc(sizeof(MainModel));
			memset(mainModel,0,sizeof(MainModel));
            SetWindowLong(hwnd,GWL_USERDATA,(LONG)mainModel);

			mainModel->view = new View(hwnd);
			mainModel->view->CreateView();

			Exception * exception1 = new Exception(pp);
			mainModel->tcpClient =  new TcpClient(exception1);
			mainModel->tcpClient->connect_to_server(hwnd);

			mainModel->hDev = CreatePopupMenu();

			AppendMenu(mainModel->hDev,MF_STRING,IDM_SET,"Connections");
			AppendMenu(mainModel->hDev,MF_SEPARATOR,IDM_SET,"");
			
            int total = appConfig->total_host;
			for(int ix =0 ;ix < total ; ix ++){
                Host * host = appConfig->hosts[ix];
			    AppendMenu(mainModel->hDev,MF_STRING,10000+ix,host->host);
			}

            return 0;
		}

		case WM_GETMINMAXINFO:{
			MINMAXINFO * mminfo=(PMINMAXINFO)lParam;  
    
			mminfo->ptMinTrackSize.x = 850;  
			mminfo->ptMinTrackSize.y = 600;  
  
			break;
		}
		case WM_NOTIFY:{
			switch (((LPNMHDR) lParam)->code) {
				case NM_DBLCLK:{
//					NM_TREEVIEW* lpnmh = (NM_TREEVIEW*) lParam;

                    char * msg = (char*)malloc(128);
                    memset(msg,0,128);

					char * buf = (char*)malloc(128);
                    memset(buf,0,128);
					SetWindowText(hwnd,msg);

					DWORD dwPos = GetMessagePos();
					POINT pt;
					pt.x = LOWORD(dwPos);
					pt.y = HIWORD(dwPos);
					ScreenToClient(mainModel->view->getTreeHwnd(), &pt);
					TVHITTESTINFO ht = {0};
					ht.pt = pt;
					ht.flags = TVHT_ONITEM;
					HTREEITEM hItem = TreeView_HitTest(mainModel->view->getTreeHwnd(), &ht);
					TVITEM ti = {0};
					ti.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
					ti.cchTextMax = 128;
					ti.pszText = buf;
					ti.hItem = hItem;
					TreeView_GetItem(mainModel->view->getTreeHwnd(), &ti);

					TreeNode * tn = (TreeNode*) ti.lParam;

					if(tn->level == 2){
					    char * scmds = (char*)malloc(sizeof(char)*128);
					    memset(scmds,0,sizeof(char) * 128);
					    sprintf(scmds,"select %d",tn->database);
					    
					    char * pppp = parse_command((char *)scmds,256);
					    mainModel->tcpClient->senddata(pppp,strlen(pppp),0);
					    
					    memset(scmds,0,sizeof(char) * 128);
					    sprintf(scmds,"keys *");
					    pppp = parse_command((char *)scmds,256);
					    mainModel->tcpClient->senddata(pppp,strlen(pppp),0);

						mainModel->selectedNode = hItem;
					}

					if(tn->level == 3){
						char * scmds = (char*)malloc(sizeof(char)*128);
					    memset(scmds,0,sizeof(char) * 128);
					    sprintf(scmds,"type %s",ti.pszText);

						SetWindowText(hwnd,scmds);
					    
					    char * pppp = parse_command((char *)scmds,256);
					    mainModel->tcpClient->senddata(pppp,strlen(pppp),0);
					}


					break;
				}
				case TVN_BEGINDRAG:
				break;

				case TVN_SELCHANGED:{
					//LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)lParam;
					//HTREEITEM treeNode = pNMTreeView->itemNew.hItem;
					//
					////char * buff = (char *)malloc(128);
					/////memset(buff,0,128);
					//
					//TVITEM item ;
					////item.pszText = buff;
					//item.mask = TVIF_PARAM;
					//item.cchTextMax = 128;
					//item.hItem = treeNode;
					//
					//TreeView_GetItem(mainModel->view->getTreeHwnd(),&item);
					//TreeNode * tn = (TreeNode*) item.lParam;
					//
					//if(tn->level=3){
					//	SetWindowText(hwnd,"zxzx");
					//}

                    //char * msg = (char*)malloc(128);
                    //memset(msg,0,128);
                    //sprintf(msg,"level:%d",tn->level);
					//SetWindowText(hwnd,msg);

//					TreeNodeModel * m = (TreeNodeModel *)(item.lParam);
					//if(strncmp("db",item.pszText,2) == 0){
					//	TV_INSERTSTRUCT tvinsert;
					//
					//	tvinsert.hParent = treeNode;
					//	tvinsert.hInsertAfter=TVI_LAST;
					//	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					//	tvinsert.item.pszText = ",,,,";
					//	tvinsert.item.iImage=2;
					//	tvinsert.item.iSelectedImage=2;
					//
					//	SendMessage(mainModel->view->getTreeHwnd(),TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					//}


					/////////////////////////////////////////////////////

	

					///////////////////////////////////////////////////////

					

					//LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)lParam;

					//LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)lParam;
					//HTREEITEM treeNode = pNMTreeView->itemNew.hItem;

					//MessageBox(hwnd,item.pszText,"Title",MB_OK);
					//
					//tvinsert.hParent = parent;
					//tvinsert.hInsertAfter=TVI_ROOT;
					//tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					//tvinsert.item.pszText = TEXT("CONNECTION->127.0.0.1:3389");
					//tvinsert.item.iImage=0;
					//tvinsert.item.iSelectedImage=0;
					//
					//SendMessage(connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

					break;
				}

				case TVN_ITEMEXPANDING:{
					break;
				}
			}

			return 0;
		}
        case WM_SIZE:
            mainModel->view->Size();
			UpdateWindow(hwnd);
            return 0;

        case WM_PAINT:
			PAINTSTRUCT _paint;
			BeginPaint(hwnd, &_paint);
			EndPaint(hwnd, &_paint);
            return 0;

        case WM_COMMAND:
			 command(hwnd,LOWORD (wParam));
                //pCtrl->Command (LOWORD (wParam));
            return 0;

        case WM_DESTROY:
			mainModel->tcpClient->close_connect();
            PostQuitMessage(0);
        return 0;

        case WM_RBUTTONDOWN:
                //GetWindowRect(hwnd,&rt);
                //pos.x= LOWORD(lParam);
                //pos.y = HIWORD(lParam);
                //TrackPopupMenu(pCtrl->hDev,TPM_RIGHTALIGN,rt.left+pos.x,rt.top+pos.y,0,hwnd,NULL);
        return 0;

		case WM_SOCKET:
			switch(LOWORD(lParam)){
				case FD_CONNECT:
					// 连接创建成功
				break;
				
				case FD_READ:{
					char *buff;
//					HWND hMsgDump;
					size_t curLeng;
					buff = mainModel->tcpClient->getbuff();
					mainModel->tcpClient->receivedata();
					
					curLeng = GetWindowTextLength(mainModel->logHwnd);
					SendMessage(mainModel->logHwnd,EM_SETSEL,curLeng,curLeng);
					SendMessage(mainModel->logHwnd,EM_REPLACESEL,FALSE,(LONG)buff);

					RedisReply * rp = read_replay(buff);

					//char * buff1 = (char*) malloc(sizeof(char) * 128);
					//memset(buff1,0,128);

					//sprintf(buff1,"type: %d",rp->type);
					//MessageBox(hwnd,buff1,"MW",MB_OK);

					if(rp->type == REPLY_MULTI){
						//TreeView_DeleteItem(mainModel->view->getTreeHwnd(),mainModel->selectedNode);
						for(int ix =0;ix < rp->bulkSize; ix ++){
					     	TV_INSERTSTRUCT tvinsert;
					     
					     	tvinsert.hParent = mainModel->selectedNode;
					     	tvinsert.hInsertAfter=TVI_LAST;
					     	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
					     	tvinsert.item.pszText = rp->bulks[ix];
					     	tvinsert.item.iImage=2;
					     	tvinsert.item.iSelectedImage=2;

							TreeNode * tn = buildTreeNode();
							tn->level = 3;
					        tvinsert.item.lParam= (LPARAM)tn;

					     	SendMessage(mainModel->view->getTreeHwnd(),TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					     }
					}
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

    return ::DefWindowProc (hwnd, message, wParam, lParam);
}


void command(HWND _hwnd,int cmd){
	MainModel * mainModel = (MainModel *)GetWindowLong(_hwnd,GWL_USERDATA);
	HINSTANCE hInst;
	//char *fname;
	RECT rt;
	POINT pt;

    switch (cmd)
    {
		//case IDM_FILE_OPEN:
		//	fname = mGetOpenFileName(_hwnd);
		//	break;
		case IDM_FILE_SAVESOURCE:

			break;
		case IDM_FILE_DUMP:
			break;
		case IDM_PLG_DEV:
			ShellExecute(_hwnd,"open","dev.exe","","",SW_NORMAL);
			break;

		case IDM_EXIT:
			SendMessage (_hwnd, WM_CLOSE, 0, 0L);
			break;

		//case IDM_VIEW_COLOR:
		//		hInst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
		//		::DialogBox (hInst,
		//					 MAKEINTRESOURCE (IDD_COLOR),
		//					 _hwnd,
		//					 (DLGPROC)ColorProc);
		//	break;

		case IDM_PROTOCAL_DEBUG:
//				hInst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
//			    DialogBox(hInst,MAKEINTRESOURCE(IDD_PROTOCAL_DEBUG),_hwnd,(DLGPROC)DebugWindowProc);
			break;

		case IDM_HELP:
			break;

		case IDM_EXE_RUN:
			break;

		case IDM_EXE_BRK:
			MessageBox(NULL,"cvcv","cvcvc",0);
			break;

		case IDM_DEV:
			GetWindowRect(_hwnd,&rt);
			GetCursorPos(&pt);
			TrackPopupMenu(mainModel->hDev,TPM_RIGHTALIGN,pt.x,pt.y,0,_hwnd,NULL);
			break;

		case IDM_STAY:
				// GetWindowRect(_hwnd,&rt);
				//SetWindowPos(_hwnd,HWND_TOPMOST,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,SWP_NOMOVE);
				//SetWindowPos(_hwnd,HWND_NOTOPMOST,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,SWP_NOMOVE);
			break;

		case IDM_ABOUT:
				hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
				DialogBox (hInst,
							 MAKEINTRESOURCE (IDD_ABOUT),
							 _hwnd,
							 AboutDlgProc);
			break;
		case IDM_VIEW_FULLSCREEN:
		break;

		case IDM_VIEW_GOTOLINE:
				hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
				::DialogBox (hInst,
							 MAKEINTRESOURCE (IDD_GOTOLINE),
							 _hwnd,
							 GoToLineDlgProc);
		break;

		//case IDM_EMU_SETBREAK:
		//		hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);//WinGetLong<HINSTANCE> (_hwnd, GWL_HINSTANCE);
		//		::DialogBox (hInst,
		//					 MAKEINTRESOURCE (IDD_BREAKPOINT),
		//					 _hwnd,
		//					 (DLGPROC)SetBrkProc);
		//break;
	}
}

LPTSTR mGetOpenFileName(HWND hwnd){
	OPENFILENAME * ofn = new OPENFILENAME;
	LPSTR fname = new char[MAX_PATH];
	ZeroMemory(fname,MAX_PATH);

	LPSTR retVal = NULL;

	ZeroMemory(ofn,sizeof(OPENFILENAME));

	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	ofn->lpstrFilter = "All Files\0*.*\0HexFiles\0*.hex\0\0";
	ofn->lpstrFile = fname;
	ofn->nMaxFile =MAX_PATH;

	ofn->Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST |
		OFN_LONGNAMES | OFN_EXPLORER |OFN_HIDEREADONLY;

	if(GetOpenFileName(ofn)){
		retVal = fname;
	}

	return retVal;
}

LPTSTR mGetSaveFileName(HWND hwnd){
	OPENFILENAME * ofn = new OPENFILENAME;
	LPSTR	fname = new char[MAX_PATH];
	ZeroMemory(fname,MAX_PATH);

	LPSTR retVal = NULL;

	ZeroMemory(ofn,sizeof(OPENFILENAME));

	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	ofn->lpstrFilter = "All Files\0*.*\0HexFiles\0*.hex\0\0";
	ofn->lpstrFile = fname;
	ofn->nMaxFile =MAX_PATH;

	ofn->Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST |
		OFN_LONGNAMES | OFN_EXPLORER |OFN_HIDEREADONLY;

	if(GetSaveFileName(ofn)){
		retVal = fname;
	}

	return retVal;
}

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_INITDIALOG:

			return TRUE;

		case WM_COMMAND:
			switch (LOWORD (wParam))
			{
				case IDOK:

				case IDCANCEL:
					::EndDialog (hwnd, 0);
				return TRUE;
			}
			break;
    }
    return FALSE;
}

BOOL CALLBACK GoToLineDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
	int lineTogo =0 ;
	switch(message)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BTN_OK:
					GetDlgItemInt(hwnd,IDC_ADDRESS,&lineTogo,FALSE);
					::EndDialog (hwnd, 0);
				break;
			}
		break;

		case WM_INITDIALOG:

		break;
	}
	return FALSE;
}