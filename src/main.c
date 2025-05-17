#include "main.h"

Application * App;
SystemResource * resource;

int MAX_DONE = 0;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
    MSG  msg;

    ginit();
    load_config();

    App = (Application *)calloc(1,sizeof(Application));
    App->dataView = (DataView *) calloc(1,sizeof(DataView));
    App->hInstance = hInst;
    App->hosts = (TreeNode**) calloc(MAX_CONNECTIONS,sizeof(TreeNode*));
    App->totalHosts=0;

    App->hostList = build_list();
    initResource();
    initpan(hInst);

    atexit(onExit);

    HWND hwndFrame = CreateWindowEx(WS_EX_LEFT,szFrameClass, TEXT (APP_TITLE),
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
            CW_USEDEFAULT, 0,preference->lwidth, preference->lheight,
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

        case WM_NEW_CONNECTION:{
            int hostIndex = (int)wParam;
            sigin_in_host(hwnd,hostIndex);
            break;
        }

        case WM_REMOVE_CONNECTION:{
            TreeNode * host = (TreeNode *)wParam;
            remove_connection(host);
            break;
        }

        case WM_UPDATE_CONNECTION_LIST:{
            int ret = DeleteMenu(App->hConnectionMenu,1,MF_BYPOSITION);
            while(ret != 0){
                ret = DeleteMenu(App->hConnectionMenu,1,MF_BYPOSITION);
            }

            appendDynamicMenu();
            break;
        }

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
            switch (msg->code) {
                case TTN_GETDISPINFO:{
                    LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
                    lpttt->hinst = App->hInstance;
                    UINT_PTR idButton = lpttt->hdr.idFrom;
                    switch(idButton){
                        case IDM_CONNECTION:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_MAIN_CONNECTION);
                        break;
                        case IDM_PREFERENCE:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_MAIN_PREFERENCE);
                        break;
                    }
                    break;
                }
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
                    }
                    break;
                }

                case TVN_SELCHANGED:{
                    LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;
                    TVITEMA item;
                    memset(&item,0,sizeof(item));
                    item.hItem = pnmtv->itemNew.hItem;
                    item.mask = TVIF_STATE;
                    item.state = TVIS_BOLD;
                    item.stateMask = TVIS_BOLD;

                    TreeNode * selected = getSelectedNode();
                    if(selected != NULL && selected->level == NODE_LEVEL_DATA){
                        SendMessage(App->view->overviewHwnd,TVM_SETITEM,0,(LPARAM)&item);
                    }

                    item.hItem = pnmtv->itemOld.hItem;
                    item.state =!TVIS_BOLD;
                    SendMessage(App->view->overviewHwnd,TVM_SETITEM,0,(LPARAM)&item);
                    break;
                }
            }

            break;
        }
        case WM_SIZE:
            onWindowResize(App->view);
            break;

        case WM_DESTROY:{
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
    init_database_view(hInstance);
    init_editor_view(hInstance);
    initSplit(hInstance);

	init_connection_pool();

    WNDCLASSEX hSplitClass;
    hSplitClass.cbSize        = sizeof(hSplitClass);
    hSplitClass.style         = 0;
    hSplitClass.lpfnWndProc   = dataViewProc;
    hSplitClass.cbClsExtra    = 0;
    hSplitClass.cbWndExtra    = 0;
    hSplitClass.hInstance     = hInstance;
    hSplitClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    hSplitClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    hSplitClass.hbrBackground = resource->brush; 
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
    mainClass.hbrBackground = resource->brush; 
    mainClass.hCursor = LoadCursor (0, IDC_ARROW);

    RegisterClassEx(&mainClass);
}

void onExit(){
    save_config(); 
}

/* heart beat to keep the tcp connection alive.*/ 
VOID CALLBACK heart_beat_timer(HWND hwnd,UINT message,UINT_PTR timer,DWORD dwTime){
    TreeNode * host = (TreeNode *) timer;
    s_db_ping(host);
}

void sigin_in_host(HWND hwnd,int hostIndex){
    Host * host = get_host_by_index(appConfig,hostIndex);
    if(host == NULL){
        return;
    }

    RedisConnection stream = init(host->host,host->port);
    if(stream == NULL){
        return;
    }

    TreeNode * hostNode = addHostNode(stream,host->name);
    int ret = s_auth(hostNode,host->password);
    if(ret == 0){
        add_host_node(host->name,hostNode);
        s_key_space(hostNode);
        App->activeHost = hostNode;

        add_node(App->hostList,hostNode);

        hostNode->timer_handler=SetTimer(NULL,(UINT_PTR)hostNode,5000,heart_beat_timer);
        showWindows(App->view);
    }
}

int match_host_node(const void * tnode,const void * target){
    if(((TreeNode *)tnode)->tid == ((TreeNode *)target)->tid){
        return 1;
    }
    return 0;
}

/* close connection */
void remove_connection(TreeNode * host){
    TreeNode * tnode = (TreeNode *)find_from_list(App->hostList,match_host_node,host);

    if(tnode == NULL){
        return;
    }

	/* stop the heart beat */
	KillTimer(NULL,tnode->timer_handler);

	/* remove tree node */
    TreeView_DeleteItem(App->view->overviewHwnd,tnode->handle);

    if(App->hostList->size == 0){
        hideWindows(App->view);
    }

	/* remove then close the connection */
	pool_remove_connection(tnode->stream->connection_id);
}

void showWindows(AppView * v){
    ShowWindow(v->overviewHwnd,SW_SHOW);
    ShowWindow(v->westSplitHwnd,SW_SHOW);
    ShowWindow(v->dataviewHwnd,SW_SHOW);
}

void hideWindows(AppView * v){
    ShowWindow(v->overviewHwnd,SW_HIDE);
    ShowWindow(v->westSplitHwnd,SW_HIDE);
    ShowWindow(v->dataviewHwnd,SW_HIDE);
}

void appendDynamicMenu(){
    if(appConfig->total_host != 0){
        AppendMenu(App->hConnectionMenu,MF_SEPARATOR,0,"");
        Host * start = appConfig->head;
        while (start != NULL){
            AppendMenu(App->hConnectionMenu,MF_STRING,(start->hostIndex + IDM_CUSTOMER_HOST+1),start->name);
            start = start->next;
        }
    }
}

void onMainFrameCreate(HWND hwnd){
    HINSTANCE hInst = App->hInstance;

    App->view = (AppView *)calloc(1,sizeof(AppView));
    AppView * app_view = App->view;
    app_view->hwnd = hwnd;

    buildToolBar(App->view);
    buildStatusBar(App->view);

    buildConnectionView(App->view);
    app_view->dataviewHwnd = CreateWindowEx(0, DATAVIEW_WINDOW,NULL,
            (!WS_VISIBLE)|WS_CHILD,
            0,0,0,0,App->view->hwnd,0,App->hInstance,NULL);

    app_view->westSplitHwnd =CreateWindowEx(0,V_SPLIT,"", 
            (!WS_VISIBLE)|WS_CHILD|WS_CLIPCHILDREN,
            0,0,0,0,App->view->hwnd,0,hInst,0);

    SendMessage(app_view->westSplitHwnd,WM_SET_PANES_HWND,(WPARAM)app_view->overviewHwnd,(LPARAM)app_view->dataviewHwnd);

    char * cbuff =(char *) calloc(1,255);
    LoadString(hInst,IDS_TB_MAIN_CONNECTION,cbuff,255);
    App->hConnectionMenu = CreatePopupMenu();
    AppendMenu(App->hConnectionMenu,MF_STRING,IDM_CONNECTION_POOL,cbuff);

    appendDynamicMenu();
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
        App->activeHost = selected;
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

    return (TreeNode*) ti.lParam;
}

// TODO rename to build host node
TreeNode * addHostNode(RedisConnection stream,char * connectionName){
    TreeNode * hostNode = build_tree_node(NULL,NODE_LEVEL_HOST);
    hostNode->stream = stream;
    sprintf(hostNode->host->host,"%s:%d",stream->host,stream->port);

    App->hosts[App->totalHosts++]=hostNode;

    return hostNode;
}

void command(HWND hwnd,int cmd){
    HINSTANCE hInst = App->hInstance;

    if(cmd > 900 && cmd < 1000){
        int hostIndex = cmd - IDM_CUSTOMER_HOST - 1;
        sigin_in_host(hwnd,hostIndex);
        return;
    }

    switch (cmd){
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
                // TODO release memory
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

OPENFILENAME * getOpenFileObject(HWND hwnd,char * filename){
    OPENFILENAME * ofn = (OPENFILENAME*) calloc(1,sizeof(OPENFILENAME));

    ofn->lStructSize = sizeof(OPENFILENAME);
    ofn->hwndOwner   = hwnd;
    ofn->hInstance   = App->hInstance;
    ofn->lpstrFilter = "All Files\0*.*\0HexFiles\0*.hex\0\0";
    ofn->lpstrFile   = filename;
    ofn->nMaxFile    = MAX_PATH;
    ofn->Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER |OFN_HIDEREADONLY;

    return ofn;
}

LPTSTR mGetOpenFileName(HWND hwnd){
    LPSTR retVal = NULL;
    LPSTR    fname = (LPSTR)calloc(MAX_PATH,sizeof(char));
    OPENFILENAME  * ofn = getOpenFileObject(hwnd,fname);

    if(GetOpenFileName(ofn)){
        retVal = fname;
    }

    free(ofn);
    return retVal;
}

LPTSTR mGetSaveFileName(HWND hwnd){
    LPSTR retVal      = NULL;
    LPSTR    fname  = (LPSTR)calloc(MAX_PATH,sizeof(char));
    OPENFILENAME  * ofn = getOpenFileObject(hwnd,fname);

    if(GetSaveFileName(ofn)){
        retVal = fname;
    }

    free(ofn);

    return retVal;
}

void onWindowResize(AppView * view){
    RECT rt;
    RECT connctionRect;
    RECT dataRect;
    RECT spliterRect;

    RECT rect;
    GetWindowRect(App->mainWindowHwnd,&rect);

    preference->lwidth  = rect.right - rect.left;
    preference->lheight = rect.bottom - rect.top;

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

    //// add progress bar 
    //view->progressBarHwnd = CreateWindowEx(0,PROGRESS_CLASS,(LPSTR)NULL,WS_CHILD|WS_VISIBLE|PBS_SMOOTH,
    //        560,4,
    //        120,20,
    //        view->statusBarHwnd,(HMENU)0,
    //        App->hInstance,NULL);

    //SendMessage(view->progressBarHwnd,PBM_SETRANGE,0,MAKELPARAM(1,100));
    //SendMessage(view->progressBarHwnd,PBM_SETSTEP,(WPARAM)1,0);
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
            //CONNECTION_AREA_WIDTH,
            preference->twidth,
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

int is_binary_data(char * stream,size_t length){
    for(size_t ix = 0;ix < length; ix ++){
        if(stream[ix] == 0){
            return 1;
        }
    }

    return 0;
}

char * encode(char * chunk,int length,int * outlength){
    char * buff = (char*)calloc(length*4+1,1);
    char * cur  = buff;

    int mlen =0;
    for(int ix = 0; ix < length; ix ++){
        char key = chunk[ix];
        if(isprint(key)){
            sprintf(cur,"%c",key);
            cur++;
            mlen ++;
        }else{
            sprintf(cur,"\\x%02X",(unsigned char)key);
            cur+=4;
            mlen += 4;
        }
    }

    if(outlength != NULL){
        * outlength = mlen;
    }

    return buff;
}

void delete_data_node(TreeNode * db_node,const char * key){
    int result = DumpMessage(0x007);
    if(result == IDYES){
        s_db_delete_key(db_node,key);
    }
}

/* set progress range */
void init_progress(int total){
    SendMessage(App->view->progressBarHwnd,PBM_SETRANGE,0,MAKELPARAM(1,total));
}


/** update data transfer progress */
void update_transfer_progress(int done,int total){
   if(done > total){
        done = total;
    }

    if(done > MAX_DONE){
        char buff[128] = {0};
        sprintf(buff,"%d/%d",done,total);
        SendMessage(App->view->statusBarHwnd,SB_SETTEXT,5,(LPARAM)buff);
        MAX_DONE = done;
    }

    //if(done >= total){
    //    SendMessage(App->view->progressBarHwnd,PBM_SETPOS,(WPARAM)0,(LPARAM)0);
    //    return;
   // }

   // SendMessage(App->view->progressBarHwnd,PBM_SETRANGE,0,MAKELPARAM(1,total));
    //SendMessage(App->view->progressBarHwnd,PBM_SETPOS,(WPARAM)done,(LPARAM)0);
}

void handle_redis_data(TreeNode * datanode,RedisReply reply){
    // TODO handle wrong response
    // TODO release memory for the view
    SendMessage(App->view->dataviewHwnd,WM_DT,(WPARAM)reply,(LPARAM)(datanode));
}

TreeNode * add_host_node(char * host_name,TreeNode * hostNode){
    TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

    //TreeNode * hostNode = build_tree_node(NULL,NODE_LEVEL_HOST);

    tvinsert.hParent             = NULL;
    tvinsert.hInsertAfter        = TVI_ROOT;
    tvinsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
    tvinsert.item.iImage         = TREE_HOST_NODE;
    tvinsert.item.iSelectedImage = TREE_HOST_NODE;
    tvinsert.item.pszText        = host_name;
    tvinsert.item.lParam         = (LPARAM)hostNode;

    HTREEITEM  handle = (HTREEITEM)SendMessage(App->view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
    hostNode->handle = handle;

    return hostNode;
}

void add_data_node(TreeNode * dbnode,RedisReply data){
    for(int ix =0; ix < dbnode->subHandleSize; ix ++){
        TreeView_DeleteItem(App->view->overviewHwnd,dbnode->subHandles[ix]);
    }

    RedisReply cursor = data->bulks[0];
    dbnode->database->cursor = atoi(cursor->bulk->content);
    SendMessage(App->view->dataviewHwnd,WM_DTT,(WPARAM)dbnode,(LPARAM)cursor->bulk->content);

    dbnode->subHandleSize= 0;
    RedisReply keydata   = data->bulks[1];

    int total = keydata->array_length;
    for(int ix = 0; ix < total; ix ++){
        RedisReply item = keydata->bulks[ix];

        item->bulk->content = item->bulk->content;
        item->bulk->length  = item->bulk->length;

        TreeNode * datanode = build_tree_node(dbnode,NODE_LEVEL_DATA);
        datanode->data->data_key   = item->bulk->content;
        datanode->data->key_length = item->bulk->length;
        datanode->stream = dbnode->stream;

        int dataType = TREE_DATA_NODE;
        DataType dt = s_db_get_data_type(dbnode->stream,item->bulk->content,item->bulk->length);

        if(dt == REDIS_STRING){
            dataType = TREE_DATA_NODE_TEXT;
        }else if(dt == REDIS_LIST){
            dataType = TREE_DATA_NODE_LIST;
        }else if (dt == REDIS_HASH){
            dataType = TREE_DATA_NODE_HASH;
        }else if (dt == REDIS_SET ){
            dataType = TREE_DATA_NODE_SET;
        }else if (dt == REDIS_ZSET){
            dataType = TREE_DATA_NODE_ZSET;
        }

        int tlen;
        char * encoded_key=encode(item->bulk->content,item->bulk->length,&tlen);

        TV_INSERTSTRUCT tvinsert;
        tvinsert.hParent             = dbnode->handle;
        tvinsert.hInsertAfter        = TVI_LAST;
        tvinsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        tvinsert.item.cchTextMax     = tlen;
        tvinsert.item.pszText        = encoded_key;
        tvinsert.item.iImage         = dataType;//TREE_DATA_NODE_HASH;
        tvinsert.item.iSelectedImage = dataType;// TREE_DATA_NODE_HASH;
        tvinsert.item.lParam         = (LPARAM)datanode;

        datanode->handle = (HTREEITEM)SendMessage(App->view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

        dbnode->subHandleSize ++;
        dbnode->subHandles[ix] = datanode->handle;

        free(encoded_key);
    }

    TreeView_Expand(App->view->overviewHwnd,dbnode->handle,TVE_EXPAND);
}

void add_database_node(TreeNode * hostNode,int dbCount){
    TV_INSERTSTRUCT tvinsert;
    
    AppView * view = App->view;
    HTREEITEM parentHandle = hostNode->handle;

    /** initialize connection specific data */
    hostNode->host->db_count = dbCount;

    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));
    tvinsert.hInsertAfter=TVI_ROOT;
    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;

    for(int dbindex =0; dbindex < dbCount;dbindex ++){
        char showName[255]={0};
        TreeNode * dbnode = build_tree_node(hostNode,NODE_LEVEL_DATABASE);
        dbnode->database->dbindex=dbindex;
        sprintf(dbnode->database->dbname,"db%d",dbindex);
        dbnode->stream = hostNode->stream;
        
        sprintf(showName,"%s",dbnode->database->dbname);
        tvinsert.item.iImage         = TREE_DATABASE_NODE;
        tvinsert.item.iSelectedImage = TREE_DATABASE_NODE;
        tvinsert.hParent= parentHandle;
        tvinsert.hInsertAfter=TVI_LAST;
        tvinsert.item.pszText= showName;
        tvinsert.item.lParam= (LPARAM)dbnode;

        HTREEITEM handle = (HTREEITEM)SendMessage(view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
        dbnode->handle = handle;

        /** add other database menu*/
        dbnode->database->db_menu  = CreatePopupMenu();
        for(int ix =0; ix < dbCount; ix ++){
            if(ix == dbindex){
                continue;
            }
            char dbname[128]={};
            sprintf(dbname,"db%d",ix);
            AppendMenu(dbnode->database->db_menu,MF_STRING,WM_MOVEDATA_CMD+ix,dbname);
        }
    }

    SendMessage(App->view->overviewHwnd,TVM_EXPAND,(WPARAM)TVE_TOGGLE,(LPARAM)(hostNode->handle));
}
