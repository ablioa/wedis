#include "main.h"

Application * App;
SystemResource * resource;

void initpan(HINSTANCE hInstance){
	init_hashview(hInstance);
	init_stringview(hInstance);
	init_listview(hInstance);
	init_setview(hInstance);
	init_zsetview(hInstance);
	init_systemview(hInstance);
	init_database_view(hInstance);
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
    hSplitClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    hSplitClass.lpszMenuName  = 0;
    hSplitClass.lpszClassName = DATAVIEW_WINDOW;
    hSplitClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&hSplitClass);

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

void onExit(){
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
	MSG  msg;

	ginit();
    save_all();
	initpan(hInst);

	App = (Application *)calloc(1,sizeof(Application));
	App->dataView = (DataView *) calloc(1,sizeof(DataView));
    App->hInstance = hInst;
	App->connectionList = init_list(NULL);

    initResource();
	HWND hwndFrame = CreateWindowEx(WS_EX_LEFT,szFrameClass, TEXT ("wedis"),
			WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			CW_USEDEFAULT, 0,400, 300,
			NULL,NULL, hInst, NULL) ;

	App->mainWindowHwnd = hwndFrame;

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
						onDataBaseSelect(selected);
						return;
					}
					break;
				}

				case NM_RCLICK:{
					if(msg->idFrom == 0){
						POINT pt;
						GetCursorPos(&pt);

						TreeNode * selected = getSelectedNode();
						if(selected != NULL && selected->level == NODE_LEVEL_HOST){
							App->selectHost = selected;
							TrackPopupMenu(App->hServerInfoMenu,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);
						}
					}
					break;
				}
			}

			break;
		}
        case WM_SIZE:
            onWindowResize(App->view);
            break;

        case WM_DESTROY:{
			onExit();
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

void showWindows(AppView * v){
	ShowWindow(v->overviewHwnd,SW_SHOW);
	ShowWindow(v->westSplitHwnd,SW_SHOW);
	ShowWindow(v->dataviewHwnd,SW_SHOW);
}

void onMainFrameCreate(HWND hwnd){
	HINSTANCE hInst = App->hInstance;

    App->view = (AppView *)calloc(1,sizeof(AppView));
    App->view->hwnd = hwnd;

	buildToolBar(App->view);
	buildStatusBar(App->view);

	buildConnectionView(App->view);
	App->view->dataviewHwnd = CreateWindowEx(0, DATAVIEW_WINDOW,NULL,
			(!WS_VISIBLE)|WS_CHILD,
			0,0,0,0,App->view->hwnd,0,App->hInstance,NULL);

	App->view->westSplitHwnd =CreateWindowEx(0,V_SPLIT,"", 
			(!WS_VISIBLE)|WS_CHILD,
			0,0,0,0,App->view->hwnd,0,hInst,0);

    SendMessage(App->view->westSplitHwnd,WM_SET_PANES_HWND,(WPARAM)App->view->overviewHwnd,(LPARAM)App->view->dataviewHwnd);

	App->hConnectionMenu = CreatePopupMenu();
	AppendMenu(App->hConnectionMenu,MF_STRING,IDM_CONNECTION_POOL,"Connections");
	AppendMenu(App->hConnectionMenu,MF_SEPARATOR,0,"");

	Host * start = appConfig->head;
	while (start != NULL){
		AppendMenu(App->hConnectionMenu,MF_STRING,(start->hostIndex + IDM_CUSTOMER_HOST+1),start->name);
		start = start->next;
	}
	
	App->hServerInfoMenu = CreatePopupMenu();
	AppendMenu(App->hServerInfoMenu,MF_STRING,IDM_SYSTEM_STAT,"Server Status");
	AppendMenu(App->hServerInfoMenu,MF_SEPARATOR,0,"");
	AppendMenu(App->hServerInfoMenu,MF_STRING,IDM_SYSTEM_STAT+1,"Commands");
	AppendMenu(App->hServerInfoMenu,MF_STRING,IDM_SYSTEM_STAT+2,"Disconnect");
}

void updateNavigationInfo(TreeNode * node){
	char tbuff[100] = {};

	TreeNode * dbnode = node->parent;
	TreeNode * hostnode = dbnode->parent;

	SendMessage(App->view->statusBarHwnd,SB_SETTEXT,1,(LPARAM)hostnode->host->host);
	SendMessage(App->view->statusBarHwnd,SB_SETTEXT,2,(LPARAM)dbnode->database->dbname);
	SendMessage(App->view->statusBarHwnd,SB_SETTEXT,3,(LPARAM)node->data->data_key);
	SendMessage(App->view->statusBarHwnd,SB_SETTEXT,4,(LPARAM)node->data->type_name);

	sprintf(tbuff,"%d",node->data->quantity);
	SendMessage(App->view->statusBarHwnd,SB_SETTEXT,5,(LPARAM)tbuff);
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

	if(selected->level == NODE_LEVEL_HOST){
		s_db_info_stats(App->activeHost,"stats");
	}

	if(selected->level == NODE_LEVEL_DATABASE){
		onDataBaseSelect(selected);
	}
}

void onDataBaseSelect(TreeNode * selected){
	if(selected == NULL){
		return;
	}

    if(selected->level == NODE_LEVEL_DATABASE){
		s_db_select(selected);
		handle_redis_data(selected,NULL);
    }
}

TreeNode * getSelectedNode(){
    DWORD dwPos = GetMessagePos();
    POINT pt;
    pt.x = LOWORD(dwPos);
    pt.y = HIWORD(dwPos);
    ScreenToClient(App->view->overviewHwnd, &pt);
    
    TVHITTESTINFO ht = {0};
    ht.pt = pt;
    ht.flags = TVHT_ONITEM;
    HTREEITEM hItem = TreeView_HitTest(App->view->overviewHwnd, &ht);
	if(hItem == NULL){
		return NULL;
	}

    TVITEM ti = {0};
    ti.mask = TVIF_HANDLE | TVIF_PARAM;
    ti.hItem = hItem;
    TreeView_GetItem(App->view->overviewHwnd, &ti);

	TreeNode * tn = (TreeNode*) ti.lParam;
	return tn;
}

TreeNode * addHostNode(RedisConnection stream,char * connectionName){
	TreeNode * hostNode = add_host_node(connectionName);
	hostNode->stream = stream;
	sprintf(hostNode->host->host,"%s:%d",stream->host,stream->port);

	return hostNode;
}

void command(HWND hwnd,int cmd){
	HINSTANCE hInst = App->hInstance;

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
		showWindows(App->view);
		return;
	}

    switch (cmd){
		case IDM_ADD:{
			DialogBox (hInst,MAKEINTRESOURCE (IDD_ADD_ENTRY),hwnd,(DLGPROC)entryDlgProc);
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
			TrackPopupMenu(App->hConnectionMenu,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);
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

		case IDM_EXE_RUN:
		case IDM_SYSTEM_STAT:{
			s_db_info_stats(App->activeHost,"stats");
			break;
		}

		case IDM_SYSTEM_STAT+2:{
			TreeNode * selected = App->selectHost;
			if(selected != NULL){
				// TODO 删除树节点,关闭连接
				TreeView_DeleteItem(App->view->overviewHwnd,selected->handle);
			}
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
	LPSTR retVal = NULL;
    OPENFILENAME * ofn = (OPENFILENAME *)calloc(1,sizeof(OPENFILENAME));
	LPSTR	fname = (LPSTR)calloc(MAX_PATH,sizeof(char));

	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner   = hwnd;
	ofn->hInstance   = App->hInstance;
	ofn->lpstrFilter = "All Files\0*.*\0HexFiles\0*.hex\0\0";
	ofn->lpstrFile   = fname;
	ofn->nMaxFile    = MAX_PATH;
	ofn->Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER |OFN_HIDEREADONLY;

	if(GetOpenFileName(ofn)){
		retVal = fname;
	}

	return retVal;
}

LPTSTR mGetSaveFileName(HWND hwnd){
	LPSTR retVal      = NULL;
	OPENFILENAME * ofn = (OPENFILENAME *)calloc(1,sizeof(OPENFILENAME));
	LPSTR	fname      = (LPSTR)calloc(MAX_PATH,sizeof(char));
	
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->hInstance = App->hInstance;
	ofn->lpstrFilter = "All Files\0*.*\0HexFiles\0*.hex\0\0";
	ofn->lpstrFile = fname;
	ofn->nMaxFile =MAX_PATH;
	ofn->Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST |OFN_LONGNAMES | OFN_EXPLORER |OFN_HIDEREADONLY;

	if(GetSaveFileName(ofn)){
		retVal = fname;
	}

	return retVal;
}

void onWindowResize(AppView * view){
    RECT rt;
    RECT connctionRect;
    RECT dataRect;
    RECT spliterRect;
    
    GetClientRect(view->hwnd,&rt);
    MoveWindow(view->toolBarHwnd,0,0,rt.right,TOOLBAR_HEIGHT,TRUE);
    MoveWindow(view->statusBarHwnd,0,rt.bottom-STATUSBAR_HEIGHT,rt.right,STATUSBAR_HEIGHT,TRUE);
    SendMessage(view->statusBarHwnd,WM_SIZE,0,0);
    
    getConnectionRect(view,&rt,&connctionRect);
    MoveWindow(view->overviewHwnd,connctionRect.left,connctionRect.top,connctionRect.right,connctionRect.bottom,TRUE);
    
    getDataRect(view,&rt,&dataRect);
    MoveWindow(view->dataviewHwnd,dataRect.left,dataRect.top,dataRect.right,dataRect.bottom,TRUE);
    
    getSpliterRect(view,&rt,&spliterRect);
    MoveWindow(view->westSplitHwnd,spliterRect.left,spliterRect.top,spliterRect.right,spliterRect.bottom,TRUE);
}

void buildToolBar(AppView * view){
	HINSTANCE hInst = App->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;
	RECT  rect;

    int buttonCount = 2;
	
	TBBUTTON tbtn[2] = {
        {(TB_CONNECTION_BUTTON), IDM_CONNECTION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_PREFERENCE_BUTTON), IDM_PREFERENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    view->toolBarHwnd = buildGeneralToolBar(view->hwnd,tbtn,buttonCount);
	
	GetWindowRect(view->toolBarHwnd,&rect);
	view->toolbarHeight = rect.bottom-rect.top;
}

void buildStatusBar(AppView * view){
    RECT trt;
    int wd[]={50,190,230,460,560,680,0};
	view->statusBarHwnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,"Wedis",view->hwnd, 1200);
	SendMessage(view->statusBarHwnd,SB_SETPARTS,7,(LPARAM)wd);
    SendMessage(view->statusBarHwnd,SB_SETMINHEIGHT,16,0);

	GetWindowRect(view->statusBarHwnd,&trt);
	view->statusbarHeight = trt.bottom-trt.top;
}

void getConnectionRect(AppView * v,RECT * rt,RECT * rect){
    rect->left   = 0;
    rect->top    = v->toolbarHeight;
    rect->right  = getConnectionWidth(v) + rect->left;
    rect->bottom = (rt->bottom - rt->top) - (v->toolbarHeight + v->statusbarHeight);
}

int getConnectionWidth(AppView * view){
    RECT rect;
    GetWindowRect(view->overviewHwnd,&rect);
    return rect.right - rect.left;
}

void getDataRect(AppView * v,RECT * rt,RECT * rect){
    int totalHeight = rt->bottom - rt->top;

    rect->left   = getConnectionWidth(v) + SPLITER_WIDTH;
    rect->top    = v->toolbarHeight;
    rect->right  = (rt->right - rt->left) - getConnectionWidth(v) - SPLITER_WIDTH;
    rect->bottom = totalHeight - v->toolbarHeight - v->statusbarHeight;
}

void getSpliterRect(AppView * v,RECT * rt,RECT * rect){
    rect->left   = getConnectionWidth(v);
    rect->top    = v->toolbarHeight;
    rect->right  = SPLITER_WIDTH;
    rect->bottom = (rt->bottom - rt->top) - v->toolbarHeight - v->statusbarHeight;
}

void buildConnectionView(AppView * view){
	RECT            rt;

	HINSTANCE hinst = App->hInstance;
	GetWindowRect(view->hwnd,&rt);

	view->overviewHwnd = CreateWindowEx(0,"SysTreeView32",0,
		WS_CHILD |WS_BORDER | (!WS_VISIBLE) | TVIF_TEXT | TVS_HASLINES  | TVS_LINESATROOT,
        	0,
            TOOLBAR_HEIGHT,
			CONNECTION_AREA_WIDTH,
            rt.bottom-rt.top-TOOLBAR_HEIGHT - STATUSBAR_HEIGHT,
            view->hwnd,NULL,hinst,0);

	SendMessage(view->overviewHwnd,TVM_SETIMAGELIST,0,(LPARAM)resource->icons);
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

HWND buildGeneralToolBar(HWND parent,TBBUTTON * tbtn,int buttonCount){
    HINSTANCE hInst = App->hInstance;

	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;
    HWND tb = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle,
			16, 16, 16, 16, parent, (HMENU) 0, hInst, NULL);

    SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) resource->icons);
    SendMessage(tb, TB_ADDBUTTONS,  (WPARAM)buttonCount,(LPARAM)tbtn);
    SendMessage(tb, TB_AUTOSIZE, 0, 0);

    ShowWindow(tb,  TRUE);
	return tb;
}
