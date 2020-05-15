#ifndef _main_h
#define _main_h

#include <windows.h>

#include "console.h"
#include "callbacks.h"

#include "resource.h"

#include "exception.h"
#include "config.h"
#include "connection.h"

#define szFrameClass "MdiFrame"

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

typedef struct{
	TcpConnection * connection;

	AppView * view;

	HMENU hDev;

	HWND logHwnd;

	HTREEITEM selectedNode;

    int UI;
}MainModel;

typedef struct{
	HWND logHwnd;

	HWND cmdHwnd;

	WNDPROC defaultCmdProc;

	MainModel * mainModel;
}ConsoleView;



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

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void initpan();

void command(HWND _hwnd,int cmd);

LPTSTR mGetOpenFileName(HWND hwnd);

LPTSTR mGetSaveFileName(HWND hwnd);

#endif

