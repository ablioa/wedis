#include "view.h"

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
	HINSTANCE hInst;
	hInst = (HINSTANCE)GetWindowLong(view->hwnd,GWL_HINSTANCE);

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
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(view->hwnd,GWL_HINSTANCE);

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

	HINSTANCE hInst = (HINSTANCE)GetWindowLong(view->hwnd,GWL_HINSTANCE);
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
    int wd[]={130,210,330,0};
	view->statusBarHwnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,"TCP-Client",view->hwnd, 200);
	SendMessage(view->statusBarHwnd,SB_SETPARTS,4,(LPARAM)wd);
    SendMessage(view->statusBarHwnd,SB_SETMINHEIGHT,20,0);

	RECT trt;
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

    HINSTANCE hinst = (HINSTANCE)GetWindowLong(view->hwnd,GWL_HINSTANCE);

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

	HINSTANCE hinst = (HINSTANCE)GetWindowLong(view->hwnd,GWL_HINSTANCE);
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

	// HIMAGELIST hImageList=ImageList_Create(14,14,ILC_COLOR16,2,10);
	// HBITMAP hBitmap = LoadBitmap(hinst,MAKEINTRESOURCE(IDB_CHIP));
	// ImageList_Add(hImageList,hBitmap,NULL);
	// SendMessage(view->connectionHwnd,TVM_SETIMAGELIST,0,(LPARAM)hImageList);

	// TV_INSERTSTRUCT tvinsert;
    // memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));
    // tvinsert.hParent = NULL;
	// tvinsert.hInsertAfter=TVI_ROOT;
	// tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	// tvinsert.item.iImage=0;
	// tvinsert.item.iSelectedImage=0;
    // tvinsert.item.pszText= "127.0.0.1:3879";

	// TreeNode * tn = buildTreeNode();
	// tn->level = 1;
	// tvinsert.item.lParam=(LPARAM)tn;
    
	// HTREEITEM hpConn=(HTREEITEM)SendMessage(view->connectionHwnd,
    //     TVM_INSERTITEM,
    //     0,(LPARAM)&tvinsert);
    
	// char * dbname = (char *)malloc(sizeof(char)*128);
	// for(int ix =0; ix < 16;ix ++){
	// 	memset(dbname,0,sizeof(char) * 128);
	// 	sprintf(dbname,"db%d",ix);

	// 	tvinsert.item.iImage=1;
	// 	tvinsert.item.iSelectedImage=1;
	// 	tvinsert.hParent=hpConn;
	// 	tvinsert.hInsertAfter=TVI_LAST;
	// 	tvinsert.item.pszText= dbname;

	// 	TreeNode * tn = buildTreeNode();
	// 	tn->level = 2;
	// 	tn->database = ix;
	// 	tvinsert.item.lParam= (LPARAM)tn;
    
	// 	SendMessage(view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	// }

	// free(dbname);
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
    TreeNode * node = (TreeNode *) malloc(sizeof(TreeNode));
	memset(node,0,sizeof(TreeNode));
	return node;
}