#include "main.h"

MainModel * mainModel;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
	MSG  msg;

    initResource();

	ginit();

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

	mainModel->mainWindowHwnd = hwndFrame;

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
					if(msg->idFrom == 0){
                        onDataNodeSelection();
					}
					break;
				}

				// 节点双击，分别处理
				case NM_DBLCLK:{
					if(msg->idFrom == 0){
						TreeNode * node = getSelectedNode();
						
						char name[255] = {0};
						sprintf(name,"node: %d %d",node->handle,node->level);
						MessageBox(NULL,name,"title",MB_OK);

                        onDataBaseSelect(node);
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
			//connection_close_connect(mainModel->connection);
            PostQuitMessage(0);
		}
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

static FILE * fout;

void initpan(HINSTANCE hInstance){
	init_hashview(hInstance);
	init_stringview(hInstance);
	init_listview(hInstance);
	init_setview(hInstance);
	init_zsetview(hInstance);
	init_systemview(hInstance);

	initSplit(hInstance);

    fout = fopen("log.txt","wa");

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

	init_working_buffer();
}

void onExit(){
}

void onMainFrameCreate(HWND hwnd){
	mainModel->view = buildAppView(hwnd);
	CreateView(mainModel->view);

	mainModel->hDev = CreatePopupMenu();

	AppendMenu(mainModel->hDev,MF_STRING,IDM_CONNECTION_POOL,"Connections");
	AppendMenu(mainModel->hDev,MF_SEPARATOR,0,"");

	Host * start = appConfig->head;
	while (start != NULL){
		AppendMenu(mainModel->hDev,MF_STRING,(start->hostIndex + IDM_CUSTOMER_HOST+1),start->name);
		start = start->next;
	}
	
	mainModel->hServerInfoMenu = CreatePopupMenu();
	AppendMenu(mainModel->hServerInfoMenu,MF_STRING,IDM_SYSTEM_STAT,"Server Status");
	AppendMenu(mainModel->hServerInfoMenu,MF_SEPARATOR,0,"");
	AppendMenu(mainModel->hServerInfoMenu,MF_STRING,IDM_SYSTEM_STAT+1,"Commands");
}

void updateNavigationInfo(TreeNode * node){
	char path[MAX_PATH] = {0};
	wsprintf(path,"db%d",node->database,node->key);
	SendMessage(mainModel->view->statusBarHwnd,SB_SETTEXT,2,(LPARAM)path);
	SendMessage(mainModel->view->statusBarHwnd,SB_SETTEXT,3,(LPARAM)node->key);
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
		redis_data_type(tn->key);
		
		updateNavigationInfo(tn);
	}
}

/**
 * 取得选中的树节点
 */ 
TreeNode * getSelectedNode(){
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
	return tn;
}

void onDataBaseSelect(TreeNode * node){
    if(node->level == 2){
		mainModel->selectedNode = node->handle;
		mainModel->database = node->database;
		
		s_db_select(node->stream,node->database);
    }
}

int packet_read(unsigned char * text,int length){
	char * binText = dumpText((char*)text,length);
	//wedis_log("packet to be processed:(%d)\n",length);
	//wedis_log("%s",binText);

	wedis_log("found: %s",text);


	RedisReply reply = read_replay(text,length);
	if(reply != NULL){
		Task * task = getTask(pool);
		dispatch(task,reply);
	}

	return 0;
}

void appendLog(const char * text){
    //fprintf(fout,"%s",text);
    //fflush(fout);
}

int check_type(char * type,char * key){
	return 1;
}

TreeNode * addHostNode(RedisConnection stream,char * connectionName){
	AppView * view = mainModel->view;

	TreeNode * hostNode = buildTreeNode(NODE_LEVEL_HOST);
	hostNode->stream = stream;

	TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

    tvinsert.hParent = NULL;
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	tvinsert.item.iImage=0;
	tvinsert.item.iSelectedImage=0;
    tvinsert.item.pszText= connectionName;
	tvinsert.item.lParam=(LPARAM)hostNode;

	HTREEITEM  handle = (HTREEITEM)SendMessage(view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	hostNode->handle = handle;

	return hostNode;
}

void command(HWND hwnd,int cmd){
	HINSTANCE hInst = mainModel->hInstance;

	if(cmd > 900 && cmd < 1000){
		Host * host = getHostByIndex(appConfig,cmd - IDM_CUSTOMER_HOST -1);
		if(host == NULL){
			return;
		}

		RedisConnection stream = init(host->host,host->port);
		if(stream == NULL){
			return;
		}

        mainModel->host = addHostNode(stream,host->name);
		s_auth(stream,host->password);
		return;
	}

    switch (cmd){

		case IDM_ADD:{
			DialogBox (hInst,MAKEINTRESOURCE (IDD_ADD_ENTRY),hwnd,(DLGPROC)entryDlgProc);
			break;
		}
		case IDM_DEBUG_GET_DATABASES:{
			// redis_key_space();
			// redis_database_count();
			//redis_info_stats();
            // redis_add_big();
			// wedis_test();
			break;
		}
		case IDM_CONNECTION_POOL:{
			DialogBox (hInst,MAKEINTRESOURCE (IDD_CONNECTION),hwnd,(DLGPROC)conectionConfigDlgProc);
			break;
		}
		
		case IDM_PREFERENCE:{
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

		case IDM_RELOAD:{
			log_message("reload data entry");
			break;
		}

		case IDM_RENAME:{
			log_message("rename data entry");
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
    
    // view->westSplitHwnd = CreateWindowEx(0,
	// 		"VSplitterCtrl","", WS_CHILD | WS_VISIBLE,
	// 		connctionRect.right,0+view->toolbarHeight,
	// 		2,rt.bottom-rt.top-view->toolbarHeight - view->statusbarHeight,
	// 		view->hwnd,NULL,hInst,NULL);
    
	// TODO 这里的200指的是属性窗口的高度,需要调整成动态的才可以
	// view->southSplitWnd = CreateWindowEx(0,
	// 		"HSplitterCtrl","", WS_CHILD | WS_VISIBLE,
	// 		connctionRect.right + SPLITER_WIDTH,
    //         rt.bottom - rt.top - view->statusbarHeight - 200 - SPLITER_WIDTH,
	// 		rt.right-rt.left - 202,
	// 		SPLITER_WIDTH,
	// 		view->hwnd,NULL,hInst,NULL);

	buildAttributeView(view);

    view->westSplitHwnd =CreateWindowEx(0,V_SPLIT,"", WS_VISIBLE|WS_CHILD,
		connctionRect.right,0+view->toolbarHeight,
		2,rt.bottom-rt.top-view->toolbarHeight - view->statusbarHeight,
		view->hwnd,0,hInst,0);

		char buff[1024] = {0};
            wsprintf(buff,"size: %d",view->toolbarHeight);
            SetWindowText(view->hwnd,buff);

    SendMessage(view->westSplitHwnd,
				WM_SET_PANES_HWND,
				(WPARAM)view->connectionHwnd,
				(LPARAM)view->dataHwnd);
	
	SendMessage(view->westSplitHwnd,
				WM_SET_PANES_HWND,
				(WPARAM)view->connectionHwnd,
				(LPARAM)view->attributeHwnd);
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
	HINSTANCE hInst = mainModel->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;
	RECT  rect;

    int buttonCount = 10;
	
	TBBUTTON tbtn[10] = {
        {(0), IDM_CONNECTION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(1), IDM_PREFERENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
		{(7), IDM_SYSTEM_STAT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(8), IDM_DEBUG_GET_DATABASES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(0), 0             , 0,               TBSTYLE_SEP,    {0}, 0, 0},
		{(2), IDM_ADD       , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_REMOVE    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(4), IDM_RELOAD    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
		{(5), IDM_RENAME    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(6), IDM_TIMING    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_MAIN));
    HIMAGELIST hIcons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, buttonCount);
	ImageList_AddMasked(hIcons, hBmp, RGB(255,255,255));

    view->toolBarHwnd = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle, 16, 16, 16, 16, view->hwnd, (HMENU) 0, hInst, NULL);
    SendMessage(view->toolBarHwnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(view->toolBarHwnd, TB_SETIMAGELIST, 0, (LPARAM) hIcons);
    SendMessage(view->toolBarHwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(view->toolBarHwnd, TB_ADDBUTTONS,       (WPARAM)buttonCount,       (LPARAM)&tbtn);
    SendMessage(view->toolBarHwnd, TB_AUTOSIZE, 0, 0);

    ShowWindow(view->toolBarHwnd,  TRUE);
	
	GetWindowRect(view->toolBarHwnd,&rect);
	view->toolbarHeight = rect.bottom-rect.top;
}

void buildStatusBar(AppView * view){
    RECT trt;
    int wd[]={50,170,200,430,530,650,0};
	view->statusBarHwnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,"Wedis",view->hwnd, 1200);
	SendMessage(view->statusBarHwnd,SB_SETPARTS,7,(LPARAM)wd);
    SendMessage(view->statusBarHwnd,SB_SETMINHEIGHT,16,0);

	SendMessage(view->statusBarHwnd, SB_SETTEXT,1, (LPARAM)"192.168.1.9:6379");
	SendMessage(view->statusBarHwnd, SB_SETTEXT,2, (LPARAM)"db1");
	SendMessage(view->statusBarHwnd, SB_SETTEXT,3, (LPARAM)"1000000000000001");
	SendMessage(view->statusBarHwnd, SB_SETTEXT,4, (LPARAM)"STRING");
	SendMessage(view->statusBarHwnd, SB_SETTEXT,5, (LPARAM)"2312 Byte");

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

	HINSTANCE hinst = mainModel->hInstance;
	GetWindowRect(view->hwnd,&rt);

	view->connectionHwnd = CreateWindowEx(0,"SysTreeView32",0,
		WS_CHILD |WS_BORDER | WS_VISIBLE | TVIF_TEXT | TVS_HASLINES  | TVS_LINESATROOT,
        	0,
            view->toolbarHeight,
			view->connectionAreaWitdh,
            rt.bottom-rt.left-view->statusbarHeight - view->statusbarHeight,
            view->hwnd,NULL,hinst,0);

	HIMAGELIST hImageList=ImageList_Create(16,16,ILC_COLOR24,2,10);
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

TreeNode * buildTreeNode(int level){
	TreeNode * node = (TreeNode *) calloc(1,sizeof(TreeNode));
	node->level = level;
    return node;
}