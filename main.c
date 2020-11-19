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

		case WM_COMMAND:{
			command(hwnd,LOWORD (wParam));
			return 0;
		}

		case WM_NOTIFY:{
			LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
				case NM_CLICK:{
					if(msg->idFrom == 0){
						TreeNode * selected = getSelectedNode();
                        onDataNodeSelection(selected);
					}
					break;
				}

				case NM_DBLCLK:{
					if(msg->idFrom == 0){
						TreeNode * selected = getSelectedNode();

						// HINSTANCE hInst = mainModel->hInstance;
						// DialogBox (hInst,MAKEINTRESOURCE (IDD_DB_SEARCH),hwnd,(DLGPROC)dbSearchDlgProc);
                        
						onDataBaseSelect(selected);
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

        case WM_DESTROY:{
			onExit();
			//connection_close_connect(mainModel->connection);
            PostQuitMessage(0);
			break;
		}

		case WM_GETMINMAXINFO:{
			MINMAXINFO * mminfo=(PMINMAXINFO)lParam;  
			mminfo->ptMinTrackSize.x = 850;
			mminfo->ptMinTrackSize.y = 600;
			break;
		}
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

	initSplit(hInstance);

    WNDCLASSEX hSplitClass;
    hSplitClass.cbSize        = sizeof(hSplitClass);
    hSplitClass.style         = 0;
    hSplitClass.lpfnWndProc   = dataViewProc;
    hSplitClass.cbClsExtra    = 0;
    hSplitClass.cbWndExtra    = 0;
    hSplitClass.hInstance     = hInstance;
    hSplitClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    hSplitClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    hSplitClass.hbrBackground = CreateSolidBrush(RGB(240,240,240)); // 240 240 240
    hSplitClass.lpszMenuName  = 0;
    hSplitClass.lpszClassName = DATAVIEW_WINDOW;
    hSplitClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&hSplitClass);

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
	char tbuff[100] = {};

	TreeNode * dbnode = node->parent;
	TreeNode * hostnode = dbnode->parent;

	SendMessage(mainModel->view->statusBarHwnd,SB_SETTEXT,1,(LPARAM)hostnode->host->host);
	SendMessage(mainModel->view->statusBarHwnd,SB_SETTEXT,2,(LPARAM)dbnode->database->dbname);
	SendMessage(mainModel->view->statusBarHwnd,SB_SETTEXT,3,(LPARAM)node->data->data_key);
	SendMessage(mainModel->view->statusBarHwnd,SB_SETTEXT,4,(LPARAM)node->data->type_name);

	sprintf(tbuff,"%d",node->data->quantity);
	SendMessage(mainModel->view->statusBarHwnd,SB_SETTEXT,5,(LPARAM)tbuff);
}

void onDataNodeSelection(TreeNode * selected){
	if(selected == NULL){
		return;
	}

	if(selected->level == NODE_LEVEL_DATA){
		TreeNode * dbnode = selected->parent;
		s_db_select(dbnode);
		s_db_data_type(selected);
		
		updateNavigationInfo(selected);
	}
}

void onDataBaseSelect(TreeNode * selected){
	if(selected == NULL){
		return;
	}

    if(selected->level == NODE_LEVEL_DATABASE){
		s_db_select(selected);
		s_db_get_data(selected);
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
	if(hItem == NULL){
		return NULL;
	}

    TVITEM ti = {0};
    ti.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
    ti.cchTextMax = 128;
    ti.pszText = buf;
    ti.hItem = hItem;
    TreeView_GetItem(mainModel->view->connectionHwnd, &ti);

	TreeNode * tn = (TreeNode*) ti.lParam;
	return tn;
}

TreeNode * addHostNode(RedisConnection stream,char * connectionName){
	AppView * view = mainModel->view;

	TreeNode * hostNode = build_tree_node(NULL,NODE_LEVEL_HOST);
	hostNode->stream = stream;

	sprintf(hostNode->host->host,"%s:%d",stream->host,stream->port);

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

        TreeNode * hostNode = addHostNode(stream,host->name);
		s_auth(hostNode,host->password);
		return;
	}

    switch (cmd){

		case IDM_ADD:{
			DialogBox (hInst,MAKEINTRESOURCE (IDD_ADD_ENTRY),hwnd,(DLGPROC)entryDlgProc);
			break;
		}
		case IDM_DEBUG_GET_DATABASES:{
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
			RedisReply dd = s_db_info_stats(mainModel->activeHost,"stats");
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

void CreateView(AppView * view){
	RECT rt;
	RECT connctionRect;

	HINSTANCE hInst = mainModel->hInstance;

	buildToolBar(view);
	buildStatusBar(view);
    
	GetClientRect(view->hwnd,&rt);
	view->dataHwnd       = CreateWindowEx(0, DATAVIEW_WINDOW,NULL,WS_CHILD | WS_VISIBLE,0,0,0,0,view->hwnd,0,mainModel->hInstance,NULL);
    // view->systemViewHwnd = CreateWindowEx(0, SYSTEM_VIEW_CLASS,NULL,WS_CHILD | WS_VISIBLE,0,0,0,0,view->hwnd,0,mainModel->hInstance,NULL);
    
	// ShowWindow(view->dataHwnd,SW_HIDE);
	// ShowWindow(view->toolBarHwnd,SW_HIDE);

    buildConnectionView(view);
    getConnectionRect(view,&rt,&connctionRect);
    
    view->westSplitHwnd =CreateWindowEx(0,V_SPLIT,"", WS_VISIBLE|WS_CHILD,
		connctionRect.right,0+view->toolbarHeight,
		2,rt.bottom-rt.top-view->toolbarHeight - view->statusbarHeight,
		view->hwnd,0,hInst,0);

    SendMessage(view->westSplitHwnd,WM_SET_PANES_HWND,(WPARAM)view->connectionHwnd,(LPARAM)view->dataHwnd);
	// SendMessage(view->westSplitHwnd,WM_SET_PANES_HWND,(WPARAM)view->connectionHwnd,(LPARAM)view->systemViewHwnd);
}

void Size(AppView * view){
    RECT rt;
    RECT connctionRect;
    RECT dataRect;
    RECT spliterRect;
    
    GetClientRect(view->hwnd,&rt);
    MoveWindow(view->toolBarHwnd,0,0,rt.right,view->toolbarHeight,TRUE);
    MoveWindow(view->statusBarHwnd,0,rt.bottom-view->statusbarHeight,rt.right,view->statusbarHeight,TRUE);
    SendMessage(view->statusBarHwnd,WM_SIZE,0,0);
    
    getConnectionRect(view,&rt,&connctionRect);
    MoveWindow(view->connectionHwnd,connctionRect.left,connctionRect.top,connctionRect.right,connctionRect.bottom,TRUE);
    
    getDataRect(view,&rt,&dataRect);
    MoveWindow(view->dataHwnd,dataRect.left,dataRect.top,dataRect.right,dataRect.bottom,TRUE);
    
    getSpliterRect(view,&rt,&spliterRect);
    MoveWindow(view->westSplitHwnd,spliterRect.left,spliterRect.top,spliterRect.right,spliterRect.bottom,TRUE);
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
    int wd[]={50,190,230,460,560,680,0};
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
    rect->bottom= totalHeight - view->toolbarHeight - view->statusbarHeight;
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
    rect->top    = totalHeight - 200 - SPLITER_WIDTH - view->statusbarHeight;
    rect->right  = totalWidth - getConnectionWidth(view) + SPLITER_WIDTH;
    rect->bottom = SPLITER_WIDTH;
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

	// ShowWindow(view->connectionHwnd,SW_HIDE);
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
