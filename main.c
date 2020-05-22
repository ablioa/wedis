#include "main.h"
#include "view.h"

MainModel * mainModel;
RenderModel * renderModel;

void initpan(HINSTANCE hInstance){

	init_hashview(hInstance);
	init_stringview(hInstance);
	init_listview(hInstance);
	init_setview(hInstance);
	init_zsetview(hInstance);

    WNDCLASSEX hSplitClass;
    hSplitClass.cbSize        = sizeof(hSplitClass);
    hSplitClass.style         = 0;
    hSplitClass.lpfnWndProc   = dataViewProc;
    hSplitClass.cbClsExtra    = 0;
    hSplitClass.cbWndExtra    = 0;
    hSplitClass.hInstance     = hInstance;
    hSplitClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    hSplitClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    hSplitClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));//(HBRUSH) GetStockObject (COLOR_BTNFACE + 1);
    hSplitClass.lpszMenuName  = 0;
    hSplitClass.lpszClassName = DATAVIEW_WINDOW;
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
    consoleWinClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));//(HBRUSH) GetStockObject (COLOR_BTNFACE + 1);
    consoleWinClass.lpszMenuName  = 0;
    consoleWinClass.lpszClassName = CONSOLE_WINDOW;
    consoleWinClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&consoleWinClass);

    WNDCLASSEX dataRenderClass;
    dataRenderClass.cbSize        = sizeof(dataRenderClass);
    dataRenderClass.style         = 0;
    dataRenderClass.lpfnWndProc   = dataRenderProc;
    dataRenderClass.cbClsExtra    = 0;
    dataRenderClass.cbWndExtra    = 0;
    dataRenderClass.hInstance     = hInstance;
    dataRenderClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    dataRenderClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    dataRenderClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));//(HBRUSH) GetStockObject (COLOR_BTNFACE + 1);
    dataRenderClass.lpszMenuName  = 0;
    dataRenderClass.lpszClassName = DATA_RENDER_WINDOW;
    dataRenderClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&dataRenderClass);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
    save_all();

	initpan(hInst);

	mainModel = (MainModel *)malloc(sizeof(MainModel));
	memset(mainModel,0,sizeof(MainModel));

	mainModel->dataView = (DataView *) malloc(sizeof(DataView));
	memset(mainModel->dataView,0,sizeof(DataView));

	renderModel = (RenderModel *) malloc(sizeof(RenderModel));
	memset(renderModel,0,sizeof(RenderModel));

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
	mainClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
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
    switch (message){
		case WM_CREATE:{
			mainModel->view = buildAppView(hwnd);
			CreateView(mainModel->view);

			mainModel->connection =  build_connection();
            connect_to_server(mainModel->connection,hwnd);

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
				case NM_CLICK:{
					char * buf = (char*)malloc(128);
                    memset(buf,0,128);

					DWORD dwPos = GetMessagePos();
					POINT pt;
					pt.x = LOWORD(dwPos);
					pt.y = HIWORD(dwPos);
					ScreenToClient(mainModel->view->connectionHwnd, &pt);

					TVHITTESTINFO ht = {0};
					ht.pt = pt;
					ht.flags = TVHT_ONITEM;
					HTREEITEM hItem = TreeView_HitTest(mainModel->view->connectionHwnd, &ht);

					TVITEM ti = {0};
					ti.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
					ti.cchTextMax = 128;
					ti.pszText = buf;
					ti.hItem = hItem;
					TreeView_GetItem(mainModel->view->connectionHwnd, &ti);

					TreeNode * tn = (TreeNode*) ti.lParam;

					if(tn->level == 3){
						char * scmds = (char*)malloc(sizeof(char)*128);
					    memset(scmds,0,sizeof(char) * 128);
					    sprintf(scmds,"type %s",ti.pszText);

						memset(mainModel->connection->key,0,256);
						sprintf(mainModel->connection->key,"%s",ti.pszText);

                        mainModel->connection->cmdType = PT_TYPE;
					    char * pppp = parse_command((char *)scmds,256);
                        connection_senddata(mainModel->connection,pppp,strlen(pppp),0);
					}

					break;
				}
				case NM_DBLCLK:{
                    char * msg = (char*)malloc(128);
                    memset(msg,0,128);

					char * buf = (char*)malloc(128);
                    memset(buf,0,128);
					SetWindowText(hwnd,msg);

					DWORD dwPos = GetMessagePos();
					POINT pt;
					pt.x = LOWORD(dwPos);
					pt.y = HIWORD(dwPos);
					ScreenToClient(mainModel->view->connectionHwnd, &pt);

					TVHITTESTINFO ht = {0};
					ht.pt = pt;
					ht.flags = TVHT_ONITEM;
					HTREEITEM hItem = TreeView_HitTest(mainModel->view->connectionHwnd, &ht);
					TVITEM ti = {0};
					ti.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
					ti.cchTextMax = 128;
					ti.pszText = buf;
					ti.hItem = hItem;
					TreeView_GetItem(mainModel->view->connectionHwnd, &ti);

					TreeNode * tn = (TreeNode*) ti.lParam;

					if(tn->level == 2){
					    char * scmds = (char*)malloc(sizeof(char)*128);
					    memset(scmds,0,sizeof(char) * 128);
					    sprintf(scmds,"select %d",tn->database);
					    
					    char * pppp = parse_command((char *)scmds,256);
                        mainModel->connection->cmdType = PT_SELECT;
                        connection_senddata(mainModel->connection,pppp,strlen(pppp),0);
					    
					    memset(scmds,0,sizeof(char) * 128);
					    sprintf(scmds,"keys *");
					    pppp = parse_command((char *)scmds,256);
                        mainModel->connection->cmdType = PT_KEYS;
                        connection_senddata(mainModel->connection,pppp,strlen(pppp),0);

						mainModel->selectedNode = hItem;
					}

					break;
				}
			}

			return 0;
		}
        case WM_SIZE:
            Size(mainModel->view);
			UpdateWindow(hwnd);
            return 0;

        case WM_PAINT:{
			PAINTSTRUCT _paint;
			BeginPaint(hwnd, &_paint);
			EndPaint(hwnd, &_paint);
        }
            return 0;

        case WM_COMMAND:
			 command(hwnd,LOWORD (wParam));
                //pCtrl->Command (LOWORD (wParam));
            return 0;

        case WM_DESTROY:
			//mainModel->tcpClient->close_connect();
            connection_close_connect(mainModel->connection);
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
					size_t curLeng;
					buff = connection_get_buffer(mainModel->connection);
                    connection_receivedata(mainModel->connection);

					curLeng = GetWindowTextLength(mainModel->logHwnd);
					SendMessage(mainModel->logHwnd,EM_SETSEL,curLeng,curLeng);
					SendMessage(mainModel->logHwnd,EM_REPLACESEL,FALSE,(LONG)buff);

					//MessageBox(hwnd,buff,buff,MB_OK);

                    RedisReply * rp = read_replay(buff);
					rp->key = mainModel->connection->key;
                    if(mainModel->connection->cmdType == PT_KEYS){
                        if(rp->type == REPLY_MULTI){
                            TVITEM ti = {0};
                            ti.mask = TVIF_HANDLE | TVIF_PARAM;
                            ti.cchTextMax = 128;
                            ti.hItem = mainModel->selectedNode;
                            TreeView_GetItem(mainModel->view->connectionHwnd, &ti);
                        
                            TreeNode * tnf = (TreeNode*) ti.lParam;
                            for(int ix =0; ix < tnf->subHandleSize; ix ++){
                                TreeView_DeleteItem(mainModel->view->connectionHwnd,tnf->subHandles[ix]);
                            }
                        
                            tnf->subHandleSize= 0;
                            
                        
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
                                tnf->subHandleSize ++;
                                tnf->subHandles[ix]=(HTREEITEM)SendMessage(mainModel->view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					         }
					    }
                    }

                    if(mainModel->connection->cmdType == PT_TYPE){
						int dataType = check_type(rp->status,rp->key);
						renderModel->data_type = dataType;
                        // SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)rp,(LPARAM)dataType);
                    }

					if(mainModel->connection->cmdType == PT_DATA){
						if(rp->type != 0){
							char cmds[128] = {0};
							sprintf(cmds,"got data,size: %d. %d",rp->bulkSize,rp->type);
							MessageBox(hwnd,cmds,"sdsd",MB_OK);
							renderModel->model = rp;

							SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)rp,(LPARAM)(renderModel->data_type));
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

    return DefWindowProc (hwnd, message, wParam, lParam);
}

// 文本框
#define TYPE_STRING "string"
// 表格单行
#define TYPE_LIST   "list"
// 表格双行
#define TYPE_HASH   "hash"
// 表格
#define TYPE_SET    "set"
// 表格
#define TYPE_ZSET   "zset"

int check_type(char * type,char * key){
	if(strcmp(type,TYPE_STRING) == 0){
		return 1;
	}

	if(strcmp(type,TYPE_LIST) == 0){
		char * command = (char *) malloc(sizeof(char) * 256);
		memset(command,0,sizeof(char) * 256);
		sprintf(command,"lrange %s 0 -1",key);
		//MessageBox(NULL,command,command,MB_OK);
		// TODO 
		mainModel->connection->cmdType = PT_DATA;
		char * pppp = parse_command((char *)command,256);
		connection_senddata(mainModel->connection,pppp,strlen(pppp),0);
		return 2;
	}
	
	if(strcmp(type,TYPE_HASH) == 0){
		return 3;
	}

	if(strcmp(type,TYPE_SET) == 0){
		return 4;
	}

	if(strcmp(type,TYPE_ZSET) == 0){
		return 5;
	}

	return 1;
}

void command(HWND _hwnd,int cmd){
	//MainModel * mainModel = (MainModel *)GetWindowLong(_hwnd,GWL_USERDATA);
	HINSTANCE hInst;
	//char *fname;
	RECT rt;
	POINT pt;

    switch (cmd){
		//case IDM_FILE_OPEN:
		//	fname = mGetOpenFileName(_hwnd);
		//	break;

        case IDM_FILE_CLOSE:
           // hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
		   // DialogBox (hInst,MAKEINTRESOURCE (IDD_COLOR),_hwnd,SetPreferenceProc);
        break;

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

		case IDM_ABOUT:
				hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
				DialogBox (hInst,
							 MAKEINTRESOURCE (IDD_ABOUT),
							 _hwnd,
							 AboutDlgProc);
			break;
	}
}

LPTSTR mGetOpenFileName(HWND hwnd){
    OPENFILENAME * ofn = (OPENFILENAME *)malloc(sizeof(OPENFILENAME));
	LPSTR	fname = (LPSTR)malloc(sizeof(char) * MAX_PATH);
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
	OPENFILENAME * ofn = (OPENFILENAME *)malloc(sizeof(OPENFILENAME));
	LPSTR	fname = (LPSTR)malloc(sizeof(char) * MAX_PATH);
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
