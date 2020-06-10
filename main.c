#include "main.h"
#include "view.h"

MainModel * mainModel;
RenderModel * renderModel;

void log_message(const char * message){
	MessageBox(NULL,message,"TITLE",MB_OK);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
	MSG  msg;

    save_all();

	initpan(hInst);

	initModel();

    mainModel->hInstance = hInst;


	HWND hwndFrame = CreateWindowEx(WS_EX_LEFT,
					szFrameClass, TEXT ("wedis"),
                    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
					CW_USEDEFAULT, 0,
					400, 300,
                    NULL,NULL, hInst, NULL) ;

    ShowWindow (hwndFrame, cmdShow) ;
    UpdateWindow (hwndFrame) ;

	HANDLE hAccel = LoadAccelerators(hInst,MAKEINTRESOURCE(IDA_RUN));
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
			onMainFrameCreate(hwnd);
			break;
		}

		case WM_GETMINMAXINFO:{
			MINMAXINFO * mminfo=(PMINMAXINFO)lParam;  
			mminfo->ptMinTrackSize.x = 850;
			mminfo->ptMinTrackSize.y = 600;
			break;
		}

		case WM_NOTIFY:{
			LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
				case NM_CLICK:{
					// 其他组件的通知消息也会通知到主窗口，因此需要过滤一下
					if(msg->idFrom == 0){
                        onDataNodeSelection();
					}
					break;
				}
				case NM_DBLCLK:{
					if(msg->idFrom == 0){
                        onDataBaseSelect(hwnd);
					}
					break;
				}
			}

			break;
		}
        case WM_SIZE:
            Size(mainModel->view);
			UpdateWindow(hwnd);
            break;

        case WM_COMMAND:{
			command(hwnd,LOWORD (wParam));
			return 0;
		}

        case WM_DESTROY:{
			onExit();
			connection_close_connect(mainModel->connection);
            PostQuitMessage(0);
		}

		case WM_SOCKET:
			networkHandle(LOWORD(lParam));
		break;

    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

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
    hSplitClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
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
    consoleWinClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
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
    dataRenderClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    dataRenderClass.lpszMenuName  = 0;
    dataRenderClass.lpszClassName = DATA_RENDER_WINDOW;
    dataRenderClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&dataRenderClass);

	WNDCLASSEX  mainClass;
	mainClass.cbSize = sizeof (WNDCLASSEX);
	mainClass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	mainClass.lpfnWndProc=MainWndProc;
	mainClass.lpszClassName = szFrameClass;
	mainClass.hInstance = hInstance;
	mainClass.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
	mainClass.hIconSm = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
	mainClass.lpszMenuName = MAKEINTRESOURCE (ID_MAIN);
	mainClass.cbClsExtra = 0;
	mainClass.cbWndExtra = 0;
	mainClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
	mainClass.hCursor = LoadCursor (0, IDC_ARROW);
	
	RegisterClassEx(&mainClass);
}

void initModel(){
	mainModel = (MainModel *)malloc(sizeof(MainModel));
	memset(mainModel,0,sizeof(MainModel));

	mainModel->dataView = (DataView *) malloc(sizeof(DataView));
	memset(mainModel->dataView,0,sizeof(DataView));

	renderModel = (RenderModel *) malloc(sizeof(RenderModel));
	memset(renderModel,0,sizeof(RenderModel));

	initTaskPool();
}

void onExit(){
}

void onMainFrameCreate(HWND hwnd){
	mainModel->view = buildAppView(hwnd);
	CreateView(mainModel->view);

	mainModel->hDev = CreatePopupMenu();

	AppendMenu(mainModel->hDev,MF_STRING,IDM_CONNECTION_POOL,"Connections");
	AppendMenu(mainModel->hDev,MF_SEPARATOR,0,"");

    int total = appConfig->total_host;
	for(int ix =0 ;ix < total ; ix ++){
        Host * host = appConfig->hosts[ix];
		host->hostIndex = IDM_CUSTOMER_HOST + ix;
		AppendMenu(mainModel->hDev,MF_STRING,(host->hostIndex),host->host);
	}
}

void onDataNodeSelection(){
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

	/** 未选中任何节点 */
	if(hItem == NULL){
		return;
	}

	TVITEM ti = {0};
	ti.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
	ti.cchTextMax = 128;
	ti.pszText = buf;
	ti.hItem = hItem;
	TreeView_GetItem(mainModel->view->connectionHwnd, &ti);

	TreeNode * tn = (TreeNode*) ti.lParam;
	if(tn->level == 3){
		char * tc = (char*)malloc(sizeof(char)*128);
        memset(tc,0,sizeof(char) * 128);
        sprintf(tc,"select %d",tn->database);
		sendRedisCommand(tc,NULL,NULL,PT_SELECT);

		char * scmds = (char*)malloc(sizeof(char)*128);
	    memset(scmds,0,sizeof(char) * 128);
	    sprintf(scmds,"type %s",ti.pszText);

		sendRedisCommand(scmds,ti.pszText,NULL,PT_TYPE);
	}	
}

void onDataBaseSelect(HWND hwnd){
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
    
    if(tn->level == 2){
        char * scmds = (char*)malloc(sizeof(char)*128);
        memset(scmds,0,sizeof(char) * 128);
        sprintf(scmds,"select %d",tn->database);

		appendLog(scmds);
        
		sendRedisCommand(scmds,NULL,NULL,PT_SELECT);


		char keycmd[256] = {0};
        memset(keycmd,0,sizeof(char) * 256);
        sprintf(keycmd,"keys *");

		appendLog(keycmd);
		
		sendRedisCommand(keycmd, NULL,NULL,PT_KEYS);

    	mainModel->selectedNode = hItem;
		mainModel->database = tn->database;
    }
}

void appendLog(char * text){
    size_t curLeng = GetWindowTextLength(mainModel->logHwnd);
	SendMessage(mainModel->logHwnd,EM_SETSEL,curLeng,curLeng);
	SendMessage(mainModel->logHwnd,EM_REPLACESEL,FALSE,(LPARAM)text);
}

void handleKeysReply(RedisReply * rp){
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
	tnf->database = mainModel->database;
    
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
		tn->database = tnf->database;
        tvinsert.item.lParam= (LPARAM)tn;
        tnf->subHandleSize ++;
        tnf->subHandles[ix]=(HTREEITEM)SendMessage(mainModel->view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
    }
}

void networkHandle(LPARAM lParam){
    switch(LOWORD(lParam)){
	    case FD_CONNECT:{
		    char * cmd = (char *) malloc(sizeof(char) * 256);
		    memset(cmd,0,sizeof(char) * 256);
		    sprintf(cmd,"auth %s",mainModel->connection->password);

	        sendRedisCommand(cmd,NULL,NULL,PT_AUTH);
	    	break;
		}
	    
	    case FD_READ:{
	    	char * buff;
	    	buff = connection_get_buffer(mainModel->connection);
            int dr_size = connection_receivedata(mainModel->connection);
			appendLog(buff);

			char * binText = dumpText((char*)buff,dr_size);
			appendLog(binText);

			Task * task = getTask(pool);
            RedisReply * rp = read_replay(buff);
			rp->key = task->dataKey;

			char msgbuff[256]={0};
			sprintf(msgbuff,"rp->type: %d",rp->type);
			MessageBox(NULL,msgbuff,"title",MB_OK);

			if(task->taskType == PT_KEYS){
                if(rp->type == REPLY_MULTI){
                    handleKeysReply(rp);
	    	    }
            }

			if(task->taskType == PT_TYPE ){
	    		int dataType = check_type(rp->status,task->dataKey);
	    		renderModel->data_type = dataType;
            }
	    
			if(task->taskType == PT_DATA){
	    		renderModel->model = rp;

				// TODO 这个判定不该有
				if(task->dataType != NULL){
					rp->dataType = (char*) malloc(sizeof(char) * 256);
					memset(rp->dataType,0,sizeof(char) * 256);
					sprintf(rp->dataType,"%s",task->dataType);

					SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)rp,(LPARAM)(renderModel->data_type));
				}
	    	}
	    }
	    break;
	    
	    case FD_WRITE:
	    break;
	    
	    case FD_CLOSE:
	    	//MessageBox(hwnd,"connction closed","title",MB_OK);
	    	//	model->excp->DumpMessage(ERR_LINK_BREAKDOWN);
	    	//	model->clt->setStatue(false);
	    	///	view->UpdateView(*model);
	    	//	EnableWindow(GetDlgItem(hwnd,IDC_MSGSEND),FALSE);
	    	//	EnableWindow(GetDlgItem(hwnd,IDC_SEND),FALSE);
	    break;
	}
}

int check_type(char * type,char * key){
	if(strcmp(type,TYPE_STRING) == 0){
		char * command = (char *) malloc(sizeof(char) * 256);
		memset(command,0,sizeof(char) * 256);
		sprintf(command,"get %s",key);

		sendRedisCommand(command,key,type,PT_DATA);

		return 1;
	}

	if(strcmp(type,TYPE_LIST) == 0){
		char * command = (char *) malloc(sizeof(char) * 256);
		memset(command,0,sizeof(char) * 256);
		sprintf(command,"lrange %s 0 -1",key);

		sendRedisCommand(command,key, type,PT_DATA);

		return 2;
	}
	
	if(strcmp(type,TYPE_HASH) == 0){
		char * command = (char *) malloc(sizeof(char) * 256);
		memset(command,0,sizeof(char) * 256);
		sprintf(command,"hgetall %s",key);

		sendRedisCommand(command,key,type,PT_DATA);
		return 3;
	}

	if(strcmp(type,TYPE_SET) == 0){
		char * command = (char *) malloc(sizeof(char) * 256);
		memset(command,0,sizeof(char) * 256);
		sprintf(command,"smembers %s",key);

		sendRedisCommand(command,key,type,PT_DATA);

		return 4;
	}

	if(strcmp(type,TYPE_ZSET) == 0){
		char * command = (char *) malloc(sizeof(char) * 256);
		memset(command,0,sizeof(char) * 256);
		sprintf(command,"zrange %s 0 -1 withscores",key);

		sendRedisCommand(command,key,type,PT_DATA);
		return 5;
	}

	return 1;
}

HTREEITEM addHostNode(char * connectionName){
	AppView * view = mainModel->view;

	TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

    tvinsert.hParent = NULL;
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	tvinsert.item.iImage=0;
	tvinsert.item.iSelectedImage=0;
    tvinsert.item.pszText= connectionName;

	TreeNode * treeNode = buildTreeNode();
	treeNode->level = 1;
	tvinsert.item.lParam=(LPARAM)treeNode;

	return (HTREEITEM)SendMessage(
		view->connectionHwnd,
        TVM_INSERTITEM,
        0,(LPARAM)&tvinsert);
}

void addDatabaseNode(HTREEITEM parentHandle){
	AppView * view = mainModel->view;
	
	TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;

	char * dbname = (char *)malloc(sizeof(char)*128);
	for(int ix =0; ix < 16;ix ++){
		memset(dbname,0,sizeof(char) * 128);
		sprintf(dbname,"db%d",ix);

		tvinsert.item.iImage=1;
		tvinsert.item.iSelectedImage=1;
		tvinsert.hParent=parentHandle;
		tvinsert.hInsertAfter=TVI_LAST;
		tvinsert.item.pszText= dbname;

		TreeNode * tn = buildTreeNode();
		tn->level = 2;
		tn->database = ix;
		tvinsert.item.lParam= (LPARAM)tn;
    
		SendMessage(view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	}

	free(dbname);
}

void addConnection(char * connectionName){
	HTREEITEM parentHandle = addHostNode(connectionName);
	addDatabaseNode(parentHandle);
}

void command(HWND hwnd,int cmd){
	HINSTANCE hInst = mainModel->hInstance;//(HINSTANCE)GetWindowLong(hwnd,GWLP_HINSTANCE);

	if(cmd > 900 && cmd < 1000){
		char buff[255] = {0};

		Host * host = getHostByIndex(appConfig,cmd);
		if(host == NULL){
			return;
		}

		// TODO 判定连接创建结果
		mainModel->connection =  build_connection(host->host,host->port,host->password);
        connect_to_server(mainModel->connection,hwnd);

		sprintf(buff,"%s:%d",host->host,host->port);
		addConnection(buff);

		return;
	}

    switch (cmd){
		case IDM_CONNECTION_POOL:{
			//hInst= (HINSTANCE)GetWindowLong(hwnd,GWLP_HINSTANCE);
			DialogBox (hInst,MAKEINTRESOURCE (IDD_CONNECTION),hwnd,(DLGPROC)conectionConfigDlgProc);
			break;
		}
		
		case IDM_FILE_CLOSE:{
			//hInst= (HINSTANCE)GetWindowLong(hwnd,GWLP_HINSTANCE);
			DialogBox (hInst,MAKEINTRESOURCE (IDD_PREFERENCE),hwnd,(DLGPROC)SetPreferenceProc);
			break;
		}

		case IDM_EXIT:{
			SendMessage (hwnd, WM_CLOSE, 0, 0L);
			break;
		}

		case IDM_CONNECTION:{
			POINT pt;
			GetCursorPos(&pt);
			TrackPopupMenu(mainModel->hDev,TPM_RIGHTALIGN,pt.x,pt.y,0,hwnd,NULL);
			break;
		}

		case IDM_TIMING:{
			log_message("timing");
			break;
		}

		case IDM_REMOVE:{
			log_message("remove");
			break;
		}
		
		case IDM_ABOUT:{
			DialogBox (hInst,MAKEINTRESOURCE (IDD_ABOUT),hwnd,(DLGPROC)AboutDlgProc);
			break;
		}
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
	ofn->hInstance = mainModel->hInstance;//(HINSTANCE)GetWindowLong(hwnd,GWLP_HINSTANCE);
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
	ofn->hInstance = mainModel->hInstance;//(HINSTANCE)GetWindowLong(hwnd,GWLP_HINSTANCE);
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

char * getOutputBuffer(int size){
    int bsize = (size/17+1) * 69;
     char * buff = ( char *) malloc(sizeof( char) * bsize);
    memset(buff,0,sizeof( char) * bsize);
    return buff;
}

char * dumpText( char * text,int len){
     char line[17]={0};
     char * buff   = getOutputBuffer(len);
     char * output =buff;

    int offset =0;
    for(int ix = 0; ix < len; ix ++){
        sprintf(output,"%02X ",(unsigned char)(text[ix]));
        output +=3;
        line[offset++]= isprint(text[ix])?text[ix]:'.';

        if(ix % 0x10 == 0x0f){
            sprintf(output,"    %s\r\n",line);
            offset=0;

            output +=22;
        }
    }

    printf("%s\n",buff);//
	return buff;
}