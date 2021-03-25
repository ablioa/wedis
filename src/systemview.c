 #include "systemview.h"

const char * cfgColNames[2]={"Property","Value"};

BOOL InitSetViewColumns1(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 2; iCol++){
        char * buff = (char *)calloc(1,255);
        strcpy(buff,cfgColNames[iCol]);
        lvc.iSubItem = iCol;
        lvc.pszText = buff;
        lvc.cx = 100;
        lvc.fmt = LVCFMT_LEFT;

        ListView_InsertColumn(hWndListView, iCol, &lvc);
        ListView_SetColumnWidth(hWndListView,iCol,(iCol+1) * 200);
    }
    
    return TRUE;
}

HWND buildStatToolBar(HWND parent){
    int buttonCount = 11;
    TBBUTTON tbtn[11] = {
        {(TB_DELETE_BUTTON), TB_CMD_FLUSH_DB, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(0), 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
        {(1), IDM_STAT_SERVER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(2), IDM_STAT_CLIENT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_STAT_MEMORY, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(4), IDM_STAT_PERSISENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(5), IDM_STAT_STATS, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(6), IDM_STAT_REPLICATION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(7), IDM_STAT_CPU, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(8), IDM_STAT_CLUSTER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(9), IDM_STAT_KEYSPACE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);;
}

BOOL updateConfigDataSet(HWND hwnd,KVPair kv){
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

        lvI.pszText = kv->array[index]->key;
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = kv->array[index]->value;
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
            model = (SystemViewModel)calloc(1,sizeof(struct system_view_model));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);
            
            GetClientRect (hwnd, &rect); 

            model->paramViewHwnd = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, App->hInstance, NULL);
            model->toolBar = buildStatToolBar(hwnd);
            
            ListView_SetExtendedListViewStyle(model->paramViewHwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            InitSetViewColumns1(model->paramViewHwnd);
            break;
        }

        case WM_COMMAND:{
            int cmd = LOWORD(wParam);
            if(cmd == TB_CMD_FLUSH_DB){
                int result = MessageBox(hwnd,"flushall will remove all data in all databases,continue?","wedis",MB_YESNOCANCEL|MB_ICONASTERISK);
                if(result == IDYES){
                    MessageBox(hwnd,"YES","title",MB_OK);
                }
            }else{
                stat_command(hwnd,LOWORD (wParam));
            }
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
    const char * statcmd = "stats";
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

    s_db_info_stats(App->activeHost,statcmd);
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
    stringViewClass.hbrBackground = resource->brush; 
    stringViewClass.lpszMenuName  = 0;
    stringViewClass.lpszClassName = SYSTEM_VIEW_CLASS;
    stringViewClass.hIconSm       = NULL;
    RegisterClassEx(&stringViewClass);
}
