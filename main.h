#ifndef _main_h
#define _main_h

#include <windows.h>

#include "hashview.h"
#include "stringview.h"
#include "listview.h"
#include "setview.h"
#include "zsetview.h"

#include "console.h"

#include "dataview.h"

#include "callbacks.h"

#include "resource/resource.h"

#include "exception.h"
#include "config.h"
#include "connection.h"

#include "queue.h"

#define szFrameClass "MdiFrame"

#define DATAVIEW_WINDOW     "DATA_VIEW_WINDOW"
#define CONSOLE_WINDOW      "CONSOLE_WINDOW"
#define DATA_RENDER_WINDOW  "DATA_RENDER_WINDOW"

#define WM_DT WM_USER+200

typedef struct{
	int data_type;
	void * model;
}RenderModel;

/** 用于提供给视图对象的数据传输对象 TODO 临时性的，要重构掉 */
extern RenderModel * renderModel;
//typedef struct{
//	HWND hwnd;
//	HWND parent;
//
//	int  cntLeft;
//	int  cntRight;
//
//	HWND hwnLeft[20];
//	HWND hwnRight[20];
//
//	int leftSize;
//	int rightSize;
//}VerticalSpliter;

//typedef struct{
//	HWND hwnd;
//	HWND parent;
//
//	int  cntTop;
//	int  cntDown;
//
//	HWND hwnTop[20];
//	HWND hwnDown[20];
//
//	int topSize;
//	int downSize;
//}SouthSpliter;

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
    
//    VerticalSpliter vs;

//    SouthSpliter southSpliter;
}AppView;

typedef struct{
	HWND logHwnd;

	HWND cmdHwnd;

	WNDPROC defaultCmdProc;
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

	//////////////////////
	HWND hashViewHwnd;
	HWND stringViewHwnd;
	HWND listViewHwnd;
	HWND setViewHwnd;
    HWND zsetViewHwnd;

	HWND visibleHwnd;
}DataView;

typedef struct{
	TcpConnection * connection;

	AppView * view;

	HMENU hDev;

	HWND logHwnd;

	DataView    * dataView;

	ConsoleView * consoleView;

	/** 选中的树节点 */
	HTREEITEM selectedNode;

	int database;

    int UI;
}MainModel;

typedef struct{
	int level;
	
	int database;

	char * key;

    HTREEITEM subHandles[1024];

    int  subHandleSize;
}TreeNode;

extern MainModel * mainModel;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void initpan();

void initModel();

void onMainFrameCreate(HWND hwnd);

void onDataNodeSelection();

void onDataBaseSelect(HWND hwnd);

void networkHandle(LPARAM lParam);

void command(HWND _hwnd,int cmd);

LPTSTR mGetOpenFileName(HWND hwnd);

LPTSTR mGetSaveFileName(HWND hwnd);


int check_type(char * type,char * key);

void log_message(const char * message);

void onExit();

#endif

