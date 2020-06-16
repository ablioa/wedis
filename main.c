#include "main.h"

MainModel * mainModel;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
	MSG  msg;

    initResource();

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

				case NM_RCLICK:{
					if(msg->idFrom == 0){
						POINT pt;
						GetCursorPos(&pt);
						TrackPopupMenu(mainModel->hServerInfoMenu,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);
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
	init_systemview(hInstance);

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
	mainModel = (MainModel *)calloc(1,sizeof(MainModel));
	mainModel->dataView = (DataView *) calloc(1,sizeof(DataView));

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

	mainModel->hServerInfoMenu = CreatePopupMenu();
	AppendMenu(mainModel->hServerInfoMenu,MF_STRING,IDM_SYSTEM_STAT,"Server Status");
	AppendMenu(mainModel->hServerInfoMenu,MF_SEPARATOR,0,"");
	AppendMenu(mainModel->hServerInfoMenu,MF_STRING,IDM_SYSTEM_STAT+1,"Commands");
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
		redis_select(tn->database);
		redis_data_type(ti.pszText);
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
		redis_select(tn->database);
		redis_keys();
    	mainModel->selectedNode = hItem;
		mainModel->database = tn->database;
    }
}

void appendLog(const char * text){
    size_t curLeng = GetWindowTextLength(mainModel->logHwnd);
	SendMessage(mainModel->logHwnd,EM_SETSEL,curLeng,curLeng);
	SendMessage(mainModel->logHwnd,EM_REPLACESEL,FALSE,(LPARAM)text);
}

void networkHandle(LPARAM lParam){
    switch(LOWORD(lParam)){
	    case FD_CONNECT:{
			redis_auth(mainModel->connection->password);
	    	break;
		}
	    
	    case FD_READ:{
	    	char * buff;
	    	buff = connection_get_buffer(mainModel->connection);
            int dr_size = connection_receivedata(mainModel->connection);
			appendLog(buff);

			char * binText = dumpText((char*)buff,dr_size);
			appendLog("\r\n-------------\r\n");
			appendLog(binText);
			appendLog("\r\n-------------\r\n");

			Task * task = getTask(pool);
            RedisReply rp = read_replay(buff);
			dispatch(task,rp);
	    }
	    break;
	    
	    case FD_WRITE:
	    break;
	    
	    case FD_CLOSE:{
		    // MessageBox(hwnd,"connction closed","title",MB_OK);
	    	// model->excp->DumpMessage(ERR_LINK_BREAKDOWN);
	    	// model->clt->setStatue(false);
	    	// view->UpdateView(*model);
	    	// EnableWindow(GetDlgItem(hwnd,IDC_MSGSEND),FALSE);
	    	// EnableWindow(GetDlgItem(hwnd,IDC_SEND),FALSE);
			break;
		}
	}
}

int check_type(char * type,char * key){
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

void command(HWND hwnd,int cmd){
	HINSTANCE hInst = mainModel->hInstance;

	if(cmd > 900 && cmd < 1000){
		

		Host * host = getHostByIndex(appConfig,cmd);
		if(host == NULL){
			return;
		}

		// TODO 判定连接创建结果
		mainModel->connection =  build_connection(host->host,host->port,host->password);
        connect_to_server(mainModel->connection,hwnd);

		// TODO 封装一下
		char connName[255] = {0};
		sprintf(connName,"%s:%d",host->host,host->port);
		HTREEITEM parentHandle = addHostNode(connName);
		mainModel->connection->hostHandle = parentHandle;

		return;
	}

    switch (cmd){
		case IDM_DEBUG_GET_DATABASES:{
			// redis_key_space();
			// redis_database_count();
			//redis_info_stats();
			break;
		}
		case IDM_CONNECTION_POOL:{
			DialogBox (hInst,MAKEINTRESOURCE (IDD_CONNECTION),hwnd,(DLGPROC)conectionConfigDlgProc);
			break;
		}
		
		case IDM_FILE_CLOSE:{
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
			TrackPopupMenu(mainModel->hDev,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);
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

		case IDM_SYSTEM_STAT:{
			redis_info_stats();
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
	ofn->hInstance = mainModel->hInstance;
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
	ofn->hInstance = mainModel->hInstance;
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

AppView * buildAppView(HWND hwnd){
    AppView * view = (AppView *)malloc(sizeof(AppView));
    ZeroMemory(view,sizeof(AppView));

    view->hwnd = hwnd;
    view->connectionAreaWitdh = CONNECTION_AREA_WIDTH;

    view->toolbarHeight = 16;
    view->statusbarHeight = 20;
    
    return view;
}

int getAttributeHeight(AppView * view){
    RECT rect;
    GetWindowRect(view->attributeHwnd,&rect);
    return rect.bottom - rect.top;
}

void CreateView(AppView * view){
	RECT rt;
	RECT connctionRect;

	HINSTANCE hInst = mainModel->hInstance;

	buildToolBar(view);
	buildStatusBar(view);
    
	GetClientRect(view->hwnd,&rt);
    buildDataView(view);
    
    buildConnectionView(view);
    getConnectionRect(view,&rt,&connctionRect);
    
    view->westSplitHwnd = CreateWindowEx(0,
			"VSplitterCtrl","", WS_CHILD | WS_VISIBLE,
			connctionRect.right,0+view->toolbarHeight,
			2,rt.bottom-rt.top-view->toolbarHeight - view->statusbarHeight,
			view->hwnd,NULL,hInst,NULL);
    
	// TODO 这里的200指的是属性窗口的高度,需要调整成动态的才可以
	view->southSplitWnd = CreateWindowEx(0,
			"HSplitterCtrl","", WS_CHILD | WS_VISIBLE,
			connctionRect.right + SPLITER_WIDTH,
            rt.bottom - rt.top - view->statusbarHeight - 200 - SPLITER_WIDTH,
			rt.right-rt.left - 202,
			SPLITER_WIDTH,
			view->hwnd,NULL,hInst,NULL);
    
    buildAttributeView(view);
}

void buildDataView(AppView * view){
	RECT rt;
    GetClientRect(view->hwnd,&rt);
    HINSTANCE hinst = mainModel->hInstance;

	int dataAreaWidth = rt.right-rt.left - 200 -2;
	view->dataHwnd=CreateWindowEx(0, DATAVIEW_WINDOW,
               NULL,
               WS_CHILD | WS_VISIBLE,
				view->connectionAreaWitdh,
				view->toolbarHeight,
				dataAreaWidth,
				rt.bottom-rt.left-view->toolbarHeight -getAttributeHeight(view),
               view->hwnd,
               0,
               hinst,
               NULL);
    
    view->systemViewHwnd=CreateWindowEx(0, SYSTEM_VIEW_CLASS,
               NULL,
               WS_CHILD | WS_VISIBLE,
				view->connectionAreaWitdh,
				view->toolbarHeight,
				dataAreaWidth,
				rt.bottom-rt.left-view->toolbarHeight -getAttributeHeight(view),
               view->hwnd,
               0,
               hinst,
               NULL);
}

void Size(AppView * view){
    RECT rt;
    RECT connctionRect;
    RECT dataRect;
    RECT spliterRect;
    RECT southSpliterRect;
    RECT attributeRect;
    
    GetClientRect(view->hwnd,&rt);
    MoveWindow(view->toolBarHwnd,0,0,rt.right,view->toolbarHeight,TRUE);
    MoveWindow(view->statusBarHwnd,0,rt.bottom-view->statusbarHeight,rt.right,view->statusbarHeight,TRUE);
    SendMessage(view->statusBarHwnd,WM_SIZE,0,0);
    
    getConnectionRect(view,&rt,&connctionRect);
    
    MoveWindow(view->connectionHwnd,connctionRect.left,connctionRect.top,connctionRect.right,connctionRect.bottom,TRUE);
    
    getDataRect(view,&rt,&dataRect);
    MoveWindow(view->dataHwnd,dataRect.left,dataRect.top,dataRect.right,dataRect.bottom,TRUE);
    MoveWindow(view->systemViewHwnd,dataRect.left,dataRect.top,dataRect.right,dataRect.bottom,TRUE);
    
    getSpliterRect(view,&rt,&spliterRect);
    MoveWindow(view->westSplitHwnd,spliterRect.left,spliterRect.top,spliterRect.right,spliterRect.bottom,TRUE);
    
    getSouthSpliterRect(view,&rt,&southSpliterRect);
    MoveWindow(view->southSplitWnd,
              southSpliterRect.left,
              southSpliterRect.top,
              southSpliterRect.right,
              southSpliterRect.bottom,
              TRUE);
    
    getAttributeRect(view,&rt,&attributeRect);
    MoveWindow(view->attributeHwnd,
              attributeRect.left,
              attributeRect.top,
              attributeRect.right,
              attributeRect.bottom,
              TRUE);
}

void buildToolBar(AppView * view){
    long oldstyle;
    TBBUTTON tbtn[3] = {};
	for(int ix = 0; ix < 3 ;ix++){
		ZeroMemory(&tbtn[ix],sizeof(TBBUTTON));

		tbtn[ix].idCommand = 0;
		tbtn[ix].iBitmap = ix;
		tbtn[ix].iString = 0;
		tbtn[ix].fsState = TBSTATE_ENABLED;
		tbtn[ix].fsStyle = TBSTYLE_BUTTON ;
		tbtn[ix].dwData = 0;
	}
	tbtn[0].idCommand = IDM_CONNECTION;
	tbtn[1].idCommand = IDM_TIMING;
	tbtn[2].idCommand = IDM_REMOVE;
	// tbtn[3].idCommand = IDM_FILE_OPEN;
	// tbtn[3].idCommand = IDM_FILE_OPEN;
	// tbtn[6].idCommand=IDM_FILE_OPEN;
	// tbtn[8].idCommand=IDM_EXE_RUN;
	// tbtn[5].idCommand = IDM_FILE_OPEN;
	// tbtn[4].fsStyle=TBSTYLE_SEP;
	// tbtn[10].fsStyle=TBSTYLE_SEP;
	// tbtn[7].fsStyle=TBSTYLE_SEP;

	HINSTANCE hInst = mainModel->hInstance;//(HINSTANCE)GetWindowLong(view->hwnd,GWLP_HINSTANCE);
	view->toolBarHwnd=CreateToolbarEx(
		view->hwnd,// parent window 
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT, // style
		1, // control identifier
		3, // number of imagelist
		hInst, // instance of exe file
		IDB_TOOLBAR_MAIN, // idenfier of image resource
		tbtn, // buttons
		3, // number of buttons
		14,  // width of button
		14,  // height of button
		14, // width of image
		14, // hright of image
		sizeof(TBBUTTON) // size of button structure
		);

	oldstyle = GetWindowLong(view->toolBarHwnd,GWL_STYLE);
	oldstyle = oldstyle  | TBSTYLE_WRAPABLE | TBSTYLE_FLAT;
	SetWindowLong(view->toolBarHwnd,GWL_STYLE,oldstyle);

	RECT trt;
	GetWindowRect(view->toolBarHwnd,&trt);
	view->toolbarHeight = trt.bottom-trt.top;
}

void buildStatusBar(AppView * view){
    RECT trt;
    int wd[]={130,210,330,0};
	view->statusBarHwnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,"Wedis",view->hwnd, 200);
	SendMessage(view->statusBarHwnd,SB_SETPARTS,4,(LPARAM)wd);
    SendMessage(view->statusBarHwnd,SB_SETMINHEIGHT,20,0);

	GetWindowRect(view->statusBarHwnd,&trt);
	view->statusbarHeight = trt.bottom-trt.top;
}

void getConnectionRect(AppView * view,RECT * rt,RECT * rect){
    rect->left=0;
    rect->top=view->toolbarHeight;
    rect->right=getConnectionWidth(view) + rect->left;
    rect->bottom= (rt->bottom - rt->top) - (view->toolbarHeight + view->statusbarHeight);
}

int getConnectionWidth(AppView * view){
    RECT rect;
    GetWindowRect(view->connectionHwnd,&rect);
    return rect.right - rect.left;
}

void getDataRect(AppView * view,RECT * rt,RECT * rect){
    int totalHeight = rt->bottom - rt->top;

    rect->left=getConnectionWidth(view) + SPLITER_WIDTH;
    rect->top=view->toolbarHeight;
    rect->right=(rt->right - rt->left) - getConnectionWidth(view) - SPLITER_WIDTH;
    rect->bottom= totalHeight - view->toolbarHeight - view->statusbarHeight - SPLITER_WIDTH- getAttributeHeight(view);
}

void getSpliterRect(AppView * view,RECT * rt,RECT * rect){
    rect->left   = getConnectionWidth(view);
    rect->top    = view->toolbarHeight;
    rect->right  = SPLITER_WIDTH;
    rect->bottom = (rt->bottom - rt->top) - (view->toolbarHeight + view->statusbarHeight);
}

void getSouthSpliterRect(AppView * view,RECT * rt,RECT * rect){

    int totalHeight = rt->bottom - rt->top;
    int totalWidth  = rt->right - rt->left;

    rect->left   = getConnectionWidth(view) + SPLITER_WIDTH;
    rect->top    = totalHeight - getAttributeHeight(view) - SPLITER_WIDTH - view->statusbarHeight;
    rect->right  = totalWidth - getConnectionWidth(view) + SPLITER_WIDTH;
    rect->bottom = SPLITER_WIDTH;
}

void getAttributeRect(AppView * view,RECT * rt,RECT * rect){
    int totalHeight = rt->bottom - rt->top;
    int totalWidth  = rt->right - rt->left;

    rect->left   = getConnectionWidth(view) + SPLITER_WIDTH;
    rect->top    = totalHeight - getAttributeHeight(view) - view->statusbarHeight;
    rect->right  = totalWidth - getConnectionWidth(view) - SPLITER_WIDTH;
    rect->bottom = getAttributeHeight(view);
}

void buildAttributeView(AppView * view){
    RECT rt;
    RECT connctionRect;

    HINSTANCE hinst = mainModel->hInstance;

	GetClientRect(view->hwnd,&rt);
	getConnectionRect(view,&rt,&connctionRect);

    view->attributeHwnd = CreateWindowEx(0, "CONSOLE_WINDOW", NULL,
                    WS_CHILD | WS_VISIBLE,
                    connctionRect.right + SPLITER_WIDTH,
                    rt.bottom - rt.top - view->statusbarHeight - 200,
                    rt.right-rt.left - 202,
                    200,
                    view->hwnd, NULL, hinst, NULL);
}

void buildConnectionView(AppView * view){
	RECT            rt;

	HINSTANCE hinst = mainModel->hInstance;//(HINSTANCE)GetWindowLong(view->hwnd,GWLP_HINSTANCE);
	GetWindowRect(view->hwnd,&rt);

	view->connectionHwnd = CreateWindowEx(0,"SysTreeView32",0,
		WS_CHILD |WS_BORDER | WS_VISIBLE | TVIF_TEXT | TVS_HASLINES  | TVS_LINESATROOT,
        	0,
            view->toolbarHeight,
			view->connectionAreaWitdh,
            rt.bottom-rt.left-view->statusbarHeight - view->statusbarHeight,
            view->hwnd,NULL,hinst,0);

	HIMAGELIST hImageList=ImageList_Create(14,14,ILC_COLOR16,2,10);
	HBITMAP hBitmap = LoadBitmap(hinst,MAKEINTRESOURCE(IDB_CHIP));
	ImageList_Add(hImageList,hBitmap,NULL);
	SendMessage(view->connectionHwnd,TVM_SETIMAGELIST,0,(LPARAM)hImageList);
}

void addTreeNode(HWND treeHwnd,HTREEITEM hParent,char * nodeName){
	TV_INSERTSTRUCT tvinsert;

	tvinsert.hParent = hParent;
	tvinsert.hInsertAfter=TVI_LAST;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvinsert.item.pszText = nodeName;
	tvinsert.item.iImage=0;
	tvinsert.item.iSelectedImage=0;

	SendMessage(treeHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
}

TreeNode * buildTreeNode(){
    return (TreeNode *) calloc(1,sizeof(TreeNode));
}