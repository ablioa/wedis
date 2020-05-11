#include "view.h"

void View::CreateView()
{
	RECT rt;
	RECT connctionRect;
	HINSTANCE hInst;
	//HWND	hwd;
	//hInst = WinGetLong<HINSTANCE> (_hwnd, GWL_HINSTANCE);
	hInst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
	//int toolHeight,stHeight;

	InitSplitterCtrl();

	buildToolBar(_hwnd);
	buildStatusBar(_hwnd);

	GetClientRect(_hwnd,&rt);
    buildDataView(_hwnd);

    buildConnectionView(_hwnd);
    getConnectionRect(&rt,&connctionRect);

    westSplitHwnd = CreateWindowEx(0,
			"VSplitterCtrl","", WS_CHILD | WS_VISIBLE,
			connctionRect.right,0+toolbarHeight,
			2,rt.bottom-rt.top-toolbarHeight - statusbarHeight,
			_hwnd,NULL,hInst,NULL);



	// TODO 这里的200指的是属性窗口的高度,需要调整成动态的才可以
	southSplitWnd = CreateWindowEx(0,
			"HSplitterCtrl","", WS_CHILD | WS_VISIBLE,
			connctionRect.right + getSpliterWidth(),
            rt.bottom - rt.top - statusbarHeight - 200 - getSpliterWidth(),
			rt.right-rt.left - 202,
			getSpliterWidth(),
			_hwnd,NULL,hInst,NULL);

    buildAttributeView(_hwnd);

    initVerticalSpliter(_hwnd,westSplitHwnd);
	addWindow(connectionHwnd,dataHwnd);
	addWindow(connectionHwnd,attributeHwnd);
	addWindow(connectionHwnd,southSplitWnd);
	setVsplitHwnd();

    initSouthSpliter(_hwnd,southSplitWnd);
	addWindowS(dataHwnd,attributeHwnd);
	setVsplitHwndS();
}

void View::Size()
{
	RECT rt;
	RECT connctionRect;
	RECT dataRect;
	RECT spliterRect;
	RECT southSpliterRect;
	RECT attributeRect;

	GetClientRect(_hwnd,&rt);
	MoveWindow(toolBarHwnd,0,0,rt.right,toolbarHeight,TRUE);
	MoveWindow(statusBarHwnd,0,rt.bottom-statusbarHeight,rt.right,statusbarHeight,TRUE);
    SendMessage(statusBarHwnd,WM_SIZE,0,0);

	getConnectionRect(&rt,&connctionRect);

	MoveWindow(connectionHwnd,connctionRect.left,connctionRect.top,connctionRect.right,connctionRect.bottom,TRUE);

	getDataRect(&rt,&dataRect);
	MoveWindow(dataHwnd,dataRect.left,dataRect.top,dataRect.right,dataRect.bottom,TRUE);

	getSpliterRect(&rt,&spliterRect);
	MoveWindow(westSplitHwnd,spliterRect.left,spliterRect.top,spliterRect.right,spliterRect.bottom,TRUE);

	getSouthSpliterRect(&rt,&southSpliterRect);
	MoveWindow(southSplitWnd,
            southSpliterRect.left,
            southSpliterRect.top,
            southSpliterRect.right,
            southSpliterRect.bottom,
            TRUE);

    getAttributeRect(&rt,&attributeRect);
	MoveWindow(attributeHwnd,
            attributeRect.left,
            attributeRect.top,
            attributeRect.right,
            attributeRect.bottom,
            TRUE);
}

void View::buildToolBar(HWND parent){
    long oldstyle;
    TBBUTTON tbtn[12] = {};
	for(int ix = 0; ix < 12 ;ix++){
		ZeroMemory(&tbtn[ix],sizeof(TBBUTTON));
		tbtn[ix].idCommand = 0;//idCommand
		tbtn[ix].iBitmap = ix;
		tbtn[ix].iString = 0;
		tbtn[ix].fsState = TBSTATE_ENABLED;
		tbtn[ix].fsStyle = TBSTYLE_BUTTON ;
		tbtn[ix].dwData = 0;
	}
	tbtn[0].idCommand = IDM_DEV;
	tbtn[1].idCommand = IDM_FILE_OPEN;
	tbtn[3].idCommand = IDM_FILE_SAVESOURCE;
	tbtn[3].idCommand = IDM_FILE_DUMP;
	tbtn[6].idCommand=IDM_VIEW_FULLSCREEN;
	tbtn[8].idCommand=IDM_EXE_RUN;
	tbtn[5].idCommand = IDM_STAY;
	tbtn[4].fsStyle=TBSTYLE_SEP;
	tbtn[10].fsStyle=TBSTYLE_SEP;
	tbtn[7].fsStyle=TBSTYLE_SEP;

	HINSTANCE hInst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
	toolBarHwnd=CreateToolbarEx(_hwnd,
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		1,
		12,
		hInst,
		IDB_TOOLBAR_MAIN,
		tbtn,
		12,
		16,16,16,16,sizeof(TBBUTTON));

	oldstyle = GetWindowLong(toolBarHwnd,GWL_STYLE);
	oldstyle = oldstyle  | TBSTYLE_WRAPABLE | TBSTYLE_FLAT;
	SetWindowLong(toolBarHwnd,GWL_STYLE,oldstyle);

	RECT trt;
	GetWindowRect(toolBarHwnd,&trt);
	toolbarHeight = trt.bottom-trt.top;
}


void View::buildStatusBar(HWND parent){
    int wd[]={130,210,330,0};
	statusBarHwnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,"TCP-Client",parent, 200);
	SendMessage(statusBarHwnd,SB_SETPARTS,4,(LPARAM)wd);
    SendMessage(statusBarHwnd,SB_SETMINHEIGHT,20,0);

	RECT trt;
	GetWindowRect(statusBarHwnd,&trt);
	statusbarHeight = trt.bottom-trt.top;
}

void View::initDataView(HWND _hdView){
	//SetBkColor(RGB(0,0,0));

    //SendMessage(_hdView,SCI_STYLESETBACK,0,RGB(255,0,0));
	//SendMessage(_hdView,SCI_STYLESETBACK,32,RGB(255,0,0));

	// fore color
	//SetForeColor(RGB(0,255,0));
	//SendMessage(_hdView,SCI_STYLESETFORE,0,RGB(0,255,0));
	//SendMessage(_hdView,SCI_STYLESETFORE,32,RGB(0,255,0));

	SendMessage(_hdView,SCI_SETMARGINMASKN,1,-1);
	SendMessage(_hdView,SCI_SETHOTSPOTACTIVEUNDERLINE,true,0);
	SendMessage(_hdView,SCI_SETHOTSPOTSINGLELINE,true,0);
//	SendMessage(_hdView,SCI_SETSCROLLWIDTH,5,0);
	SendMessage(_hdView,SCI_SETHOTSPOTACTIVEBACK,true,RGB(255,0,0));
	SendMessage(_hdView,SCI_SETHOTSPOTACTIVEFORE,true,RGB(0,255,0));
	SendMessage(_hdView,SCI_USEPOPUP,false,0);

	SendMessage(_hdView,SCI_SETHSCROLLBAR,false,0);
	SendMessage(_hdView,SCI_SETVIEWWS,SCWS_INVISIBLE,0);
	SendMessage(_hdView,SCI_STYLESETFONT,32,(LPARAM)"Courier New");
	SendMessage(_hdView,SCI_STYLESETSIZE,32,10);
	SendMessage(_hdView,SCI_STYLESETBOLD,32,true);

    SendMessage(_hdView,SCI_SETREADONLY,true,0);
}

//void View::buildDataView(HWND _hwnd){
//    RECT rt;
//    GetClientRect(_hwnd,&rt);
//    HINSTANCE hinst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
//
//    int dataAreaWidth = rt.right-rt.left - 200 -2;
//    dataHwnd = CreateWindowEx(0,
//			"Scintilla","",
//			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN | !WS_BORDER,
//			connectionAreaWitdh,
//			toolbarHeight,
//			dataAreaWidth,
//			rt.bottom-rt.left-toolbarHeight -getAttributeHeight(),
//			_hwnd,NULL,hinst,NULL);
//
//    initDataView(dataHwnd);
//}

void View::buildDataView(HWND hwnd){
	RECT rt;
    GetClientRect(_hwnd,&rt);
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);

	int dataAreaWidth = rt.right-rt.left - 200 -2;

    //dataHwnd=CreateWindowEx(0, "EDIT",
    //           NULL, 
    //           WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
    //           ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
	//			connectionAreaWitdh,
	//			toolbarHeight,
	//			dataAreaWidth,
	//			rt.bottom-rt.left-toolbarHeight -getAttributeHeight(),
    //           hwnd,
    //           0,
    //           hinst,
    //           NULL);

	dataHwnd=CreateWindowEx(0, "wowowo",
               NULL, 
               WS_CHILD | WS_VISIBLE,
				connectionAreaWitdh,
				toolbarHeight,
				dataAreaWidth,
				rt.bottom-rt.left-toolbarHeight -getAttributeHeight(),
               hwnd,
               0,
               hinst,
               NULL);

	initDataView(dataHwnd);

}


void View::initVerticalSpliter(HWND _parent,HWND _hwnd){
	vs.hwnd = _hwnd;
	vs.parent = _parent;

	vs.cntLeft  = 0;
	vs.cntRight = 0;

    for(int ix =0;ix < 20 ;ix ++){
		vs.hwnLeft[ix]=NULL;
		vs.hwnRight[ix]=NULL;
	}
}

void View::initSouthSpliter(HWND _parent,HWND _hwnd){
    southSpliter.hwnd = _hwnd;
	southSpliter.parent = _parent;

	southSpliter.cntTop  = 0;
	southSpliter.cntDown = 0;

    for(int ix =0;ix < 20 ;ix ++){
		southSpliter.hwnTop[ix]=NULL;
		southSpliter.hwnDown[ix]=NULL;
	}
}

void View::addWindow(HWND hLeft,HWND hRight){
    if(hLeft != NULL)
		vs.hwnLeft[vs.cntLeft++]=hLeft;

	if(hRight != NULL)
		vs.hwnRight[vs.cntRight++]=hRight;
}

void View::addWindowS(HWND hTop,HWND hDown){
    if(hTop != NULL)
		southSpliter.hwnTop[southSpliter.cntTop++]=hTop;

	if(hDown != NULL)
		southSpliter.hwnDown[southSpliter.cntDown++]=hDown;
}

void View::setVsplitHwndS(){
    SendMessage(southSpliter.hwnd,WM_SET_PANES_HWND,(long)southSpliter.hwnTop,(long)southSpliter.hwnDown);
    SendMessage(southSpliter.hwnd,WM_SET_BORDERS_COL,RGB(255,240,240),0);
	SendMessage(southSpliter.hwnd,WM_SET_BORDERS_MIN,22,33);
}

void View::setVsplitHwnd(){
    SendMessage(vs.hwnd,WM_SET_PANES_HWND,(long)vs.hwnLeft,(long)vs.hwnRight);
    SendMessage(vs.hwnd,WM_SET_BORDERS_COL,RGB(240,240,240),0);
	SendMessage(vs.hwnd,WM_SET_BORDERS_MIN,22,33);
}

void View::getConnectionRect(RECT * rt,RECT * rect){
    rect->left=0;
    rect->top=toolbarHeight;
    rect->right=getConnectionWidth() + rect->left;
    rect->bottom= (rt->bottom - rt->top) - (toolbarHeight + statusbarHeight);
}

int View::getConnectionWidth(){
    RECT rect;
    GetWindowRect(connectionHwnd,&rect);
    return rect.right - rect.left;
}

void View::getDataRect(RECT * rt,RECT * rect){
    int totalHeight = rt->bottom - rt->top;

    rect->left=getConnectionWidth() + getSpliterWidth();
    rect->top=toolbarHeight;
    rect->right=(rt->right - rt->left) - getConnectionWidth() - getSpliterWidth();
    rect->bottom= totalHeight - toolbarHeight - statusbarHeight - getSpliterWidth()- getAttributeHeight();
}

int View::getAttributeHeight(){
    RECT rect;
    GetWindowRect(attributeHwnd,&rect);
    return rect.bottom - rect.top;
}

void View::getSpliterRect(RECT * rt,RECT * rect){
    rect->left   = getConnectionWidth();
    rect->top    = toolbarHeight;
    rect->right  = getSpliterWidth();
    rect->bottom = (rt->bottom - rt->top) - (toolbarHeight + statusbarHeight);
}

void View::getSouthSpliterRect(RECT * rt,RECT * rect){

    int totalHeight = rt->bottom - rt->top;
    int totalWidth  = rt->right - rt->left;

    rect->left   = getConnectionWidth() + getSpliterWidth();
    rect->top    = totalHeight - getAttributeHeight() - getSpliterWidth() - statusbarHeight;
    rect->right  = totalWidth - getConnectionWidth() + getSpliterWidth();
    rect->bottom = getSpliterWidth();
}

void View::getAttributeRect(RECT * rt,RECT * rect){
    int totalHeight = rt->bottom - rt->top;
    int totalWidth  = rt->right - rt->left;

    rect->left   = getConnectionWidth() + getSpliterWidth();
    rect->top    = totalHeight - getAttributeHeight() - statusbarHeight;
    rect->right  = totalWidth - getConnectionWidth() - getSpliterWidth();
    rect->bottom = getAttributeHeight();
}

void View::buildAttributeView(HWND parent){
    RECT rt;
    RECT connctionRect;

    HINSTANCE hinst = (HINSTANCE)GetWindowLong(parent,GWL_HINSTANCE);

	GetClientRect(_hwnd,&rt);
	getConnectionRect(&rt,&connctionRect);

    //attributeHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "SysListView32", NULL,
    //                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_SORTASCENDING,
    //                connctionRect.right + getSpliterWidth(),
    //                rt.bottom - rt.top - statusbarHeight - 200,
    //                rt.right-rt.left - 202,
    //                200,
    //                parent, NULL, hinst, NULL);

	//dataHwnd=CreateWindowEx(0, "wowowo",
    //           NULL, 
    //           WS_CHILD | WS_VISIBLE,
	//			connectionAreaWitdh,
	//			toolbarHeight,
	//			dataAreaWidth,
	//			rt.bottom-rt.left-toolbarHeight -getAttributeHeight(),
    //           hwnd,
    //           0,
    //           hinst,
    //           NULL);

    attributeHwnd = CreateWindowEx(0, "CONSOLE_WINDOW", NULL,
                    WS_CHILD | WS_VISIBLE,
                    connctionRect.right + getSpliterWidth(),
                    rt.bottom - rt.top - statusbarHeight - 200,
                    rt.right-rt.left - 202,
                    200,
                    parent, NULL, hinst, NULL);

	//"CONSOLE_WINDOW"

	//LV_COLUMN lvc;
	//lvc.mask=LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT; //LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM
    //lvc.pszText = "AGE";
	//lvc.cx=125;
	//lvc.iSubItem=0;
    //SendMessage(attributeHwnd, LVM_INSERTCOLUMN, 0, (long)&lvc);
	//
	//lvc.pszText = "NAME";
	//lvc.cx=125;
	//lvc.iSubItem=1;
    //SendMessage(attributeHwnd, LVM_INSERTCOLUMN, 0, (long)&lvc);
    //// TODO 后面继续初始化
	//
	//LV_ITEM lvi;
    //lvi.mask = LVIF_TEXT | LVIF_PARAM ;
	//lvi.iItem = 0;
    //lvi.lParam = 0;
	//lvi.iSubItem = 0;
    //lvi.pszText="wangzh";
	////lvi.iImage = 0;
    //SendMessage(attributeHwnd, LVM_INSERTITEM, 0, (long)&lvi);
	//
	////lvi.mask = LVIF_TEXT | LVIF_PARAM; LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM
	//lvi.iItem = 1;
    //lvi.lParam = 0;
    //lvi.iSubItem = 1;
    //lvi.pszText="12";
	////lvi.iImage = 0;
    //SendMessage(attributeHwnd, LVM_INSERTITEM, 0, (long)&lvi);
}

void View::buildConnectionView(HWND _hwnd){
	TV_INSERTSTRUCT tvinsert;
	HIMAGELIST      hImageList;
	HBITMAP         hBitmap;
	RECT            rt;

	HINSTANCE hinst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
	GetWindowRect(_hwnd,&rt);

	connectionHwnd = CreateWindowEx(0,"SysTreeView32",0,
		WS_CHILD |WS_BORDER | WS_VISIBLE | TVIF_TEXT | TVS_HASLINES  | TVS_LINESATROOT,
        	0,toolbarHeight,
			connectionAreaWitdh,rt.bottom-rt.left-statusbarHeight - statusbarHeight,
        _hwnd,NULL,hinst,0);

	hImageList=ImageList_Create(16,16,ILC_COLOR16,2,10);
	hBitmap = LoadBitmap(hinst,MAKEINTRESOURCE(IDB_CHIP));
	ImageList_Add(hImageList,hBitmap,NULL);
	SendMessage(connectionHwnd,TVM_SETIMAGELIST,0,(LPARAM)hImageList);


	//TreeNodeModel * model = buildTreeNodeModel();
	//model->type =1;

	tvinsert.hParent = NULL;
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	tvinsert.item.pszText = TEXT("CONNECTION->127.0.0.1:3389");
	tvinsert.item.iImage=0;
	tvinsert.item.iSelectedImage=0;

	TreeNode * tn = buildTreeNode();
	tn->level = 1;
	tvinsert.item.lParam=(LPARAM)tn;

//	tvinsert.item.lParam = (long)model;

	HTREEITEM hpConn=(HTREEITEM)SendMessage(connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	HTREEITEM hpDb;

	char * dbname = (char *)malloc(sizeof(char)*128);
	for(int ix =0; ix < 16;ix ++){
		memset(dbname,0,sizeof(char) * 128);

		sprintf(dbname,"db%d",ix);

		tvinsert.item.iImage=1;
		tvinsert.item.iSelectedImage=1;
		tvinsert.hParent=hpConn;
		tvinsert.hInsertAfter=TVI_LAST;
		tvinsert.item.pszText= dbname;
		TreeNode * tn = buildTreeNode();
		tn->level = 2;
		tn->database = ix;
		tvinsert.item.lParam= (LPARAM)tn;

		SendMessage(connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

	}

	free(dbname);
}

//extern TreeNode * treeNodes;
//
//TreeNodeModel * buildTreeNodeModel(){
//    TreeNode * ret = (TreeNode*)malloc(sizeof(TreeNode));
//	memset(ret,0,sizeof(TreeNode));
//	return ret;
//}
//
//void release(){
//}


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