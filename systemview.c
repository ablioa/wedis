 #include "systemview.h"

const char * cfgColNames[2]={"Property","Value"};
HWND buildStatToolBar(HWND parent){
	HINSTANCE hInst = mainModel->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 9;
	TBBUTTON tbtn[10] = {
        {(0), IDM_STAT_SERVER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(1), IDM_STAT_CLIENT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(2), IDM_STAT_MEMORY, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_STAT_PERSISENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(4), IDM_STAT_STATS, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(5), IDM_STAT_REPLICATION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(6), IDM_STAT_CPU, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(7), IDM_STAT_CLUSTER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(8), IDM_STAT_KEYSPACE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_STATTB));
    HIMAGELIST hIcons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, buttonCount);
	ImageList_AddMasked(hIcons, hBmp, RGB(255,255,255));

    HWND tb = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle, 16, 16, 16, 16, parent, (HMENU) 0, hInst, NULL);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) hIcons);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_ADDBUTTONS,       (WPARAM)buttonCount,       (LPARAM)&tbtn);
    SendMessage(tb, TB_AUTOSIZE, 0, 0);

    ShowWindow(tb,  TRUE);

    return tb;
}

BOOL InitSetViewColumns1(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;

	char * valName;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 2; iCol++){

        // 注意内存释放
		valName = (char *) malloc(sizeof(char) * 128);
		memset(valName,0,sizeof(char) * 128);
		strcpy(valName,cfgColNames[iCol]);

        lvc.iSubItem = iCol;
        lvc.pszText = valName;
        lvc.cx = 100;

        if ( iCol < 2 ){
            lvc.fmt = LVCFMT_LEFT;
		}
        else{
            lvc.fmt = LVCFMT_RIGHT;
		}

        if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1){
            return FALSE;
		}

        ListView_SetColumnWidth(hWndListView,iCol,(iCol+1) * 200);
    }
    
    return TRUE;
}

BOOL updateConfigDataSet(HWND hwnd,KVPair kv){
    char value[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);
    
    for (int index = 0; index < kv->count; index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        memset(value,0,256);
        sprintf(value,"%d",kv->array[index]->value);

        lvI.pszText = kv->array[index]->key;
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = value;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

LRESULT CALLBACK SystemViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    SystemViewModel model = (SystemViewModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            model = (StringViewModel*)calloc(1,sizeof(StringViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);
            
            GetClientRect (hwnd, &rect); 

	        model->paramViewHwnd = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, mainModel->hInstance, NULL);
            model->toolBar = buildStatToolBar(hwnd);
            
            ListView_SetExtendedListViewStyle(model->paramViewHwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
			InitSetViewColumns1(model->paramViewHwnd);
		    break;
		}

        case WM_COMMAND:{
			stat_command(hwnd,LOWORD (wParam));
			return 0;
		}

        case WM_DT:{
            RedisReply data     = (RedisReply) wParam;
            //TreeNode * datanode = (TreeNode *) lParam;

            KVPair kp = parseKVPair(data->bulk->content);

            updateConfigDataSet(model->paramViewHwnd,kp);
            break;
        }
       
		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
            MoveWindow(hwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			// MoveWindow(model->paramViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
			MoveWindow(model->paramViewHwnd,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

void stat_command(HWND hwnd,int cmd){
    char * statcmd = "stats";
    switch (cmd){
		case IDM_STAT_SERVER:{statcmd = "server";break;}
        case IDM_STAT_CLIENT:{statcmd = "clients";break;}
        case IDM_STAT_MEMORY:{statcmd = "memory";break;}
        case IDM_STAT_PERSISENCE:{statcmd = "persistence";break;}
        case IDM_STAT_STATS:{statcmd = "stats";break;}
        case IDM_STAT_REPLICATION:{statcmd = "replication";break;}
        case IDM_STAT_CPU:{statcmd = "cpu";break;}
        case IDM_STAT_CLUSTER:{statcmd = "cluster";break;}
        case IDM_STAT_KEYSPACE:{statcmd = "keyspace";break;}
    }

    RedisReply dd = s_db_info_stats(mainModel->activeHost,statcmd);
};

void init_systemview(HINSTANCE hInstance){
	WNDCLASSEX stringViewClass;

    stringViewClass.cbSize        = sizeof(WNDCLASSEX);
    stringViewClass.style         = 0;
    stringViewClass.lpfnWndProc   = SystemViewWndProc;
    stringViewClass.cbClsExtra    = 0;
    stringViewClass.cbWndExtra    = 0;
    stringViewClass.hInstance     = hInstance;
    stringViewClass.hIcon         = NULL;
    stringViewClass.hCursor       = NULL;
    stringViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    stringViewClass.lpszMenuName  = 0;
    stringViewClass.lpszClassName = SYSTEM_VIEW_CLASS;
    stringViewClass.hIconSm       = NULL;
    RegisterClassEx(&stringViewClass);
}
