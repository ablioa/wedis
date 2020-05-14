#ifndef _view_h
#define _view_h

#define _WIN32_IE 0x0600

#include <windows.h>
#include "resource.h"
#include "callbacks.h"

#include <stdio.h>

#include <commctrl.h>

#define MAX_SIZE	20
#define CONNECTION_AREA_WIDTH 200
#define SPLITER_WIDTH 2

#define DATAVIEW_WINDOW     "wowowo"
#define CONSOLE_WINDOW      "CONSOLE_WINDOW"
#define DATA_RENDER_WINDOW  "DATA_RENDER_WINDOW"

typedef struct{
	HWND hwnd;
	HWND parent;

	int  cntLeft;
	int  cntRight;

	HWND hwnLeft[20];
	HWND hwnRight[20];

	int leftSize;
	int rightSize;
}VerticalSpliter;

typedef struct{
	HWND hwnd;
	HWND parent;

	int  cntTop;
	int  cntDown;

	HWND hwnTop[20];
	HWND hwnDown[20];

	int topSize;
	int downSize;
}SouthSpliter;

typedef struct{
	HWND dataViewHwnd;
	HWND exportBtnHwnd;
	HWND saveBtnHwnd;
	HWND viewTypeHwnd;

	HWND reloadBtnHwnd;
	HWND removeBtnHwnd;
	HWND ttlBtnHwnd;
	HWND renameBtnHwnd;
	HWND keyEditHwnd;

	WNDPROC ttlBtnProc;
	WNDPROC exportBtnProc;
}DataView;

typedef struct{
	int level;
	int database;
	char * key;

    HTREEITEM subHandles[1024];
    int  subHandleSize;
}TreeNode;

TreeNode * buildTreeNode();

typedef struct{
    HWND hwnd;
    
    HWND toolBarHwnd;
    HWND statusBarHwnd;
    
    HWND connectionHwnd;
    HWND dataHwnd;
    HWND attributeHwnd;
    
    HWND westSplitHwnd;
    HWND southSplitWnd;
    
    int toolbarHeight;
    int statusbarHeight;
    
    int connectionAreaWitdh;
    
    VerticalSpliter vs;
    SouthSpliter southSpliter;
}AppView;

AppView * buildAppView(HWND hwnd);

void buildToolBar(AppView * appView);

void buildStatusBar(AppView * view);

void buildDataView(AppView * view);

void buildConnectionView(AppView * view);

void getConnectionRect(AppView * view,RECT * rt,RECT * rect);

int getConnectionWidth(AppView * view);

void buildAttributeView(AppView * view);

int getAttributeHeight(AppView * view);

void CreateView(AppView * appView);

void Size(AppView * appView);

void getDataRect(AppView * view,RECT * rt,RECT * rect);

void getSpliterRect(AppView * view,RECT * rt,RECT * rect);

void getSouthSpliterRect(AppView * view,RECT * rt,RECT * rect);

void getAttributeRect(AppView * view,RECT * rt,RECT * rect);

void initVerticalSpliter(AppView * view,HWND _parent,HWND _hwnd);

void initSouthSpliter(AppView * view,HWND _parent,HWND _hwnd);

void addWindow(AppView * view,HWND hLeft,HWND hRight);

void addWindowS(AppView * view,HWND hTop,HWND hDown);

void setVsplitHwndS(AppView * view);

void setVsplitHwnd(AppView * view);

#endif
