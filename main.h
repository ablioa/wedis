#ifndef _main_h
#define _main_h

#include <windows.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "hashview.h"
#include "stringview.h"
#include "listview.h"
#include "setview.h"
#include "zsetview.h"

#include "wedis.h"

#include "console.h"

#include "dataview.h"
#include "system.h"

#include "callbacks.h"

#include "resource/resource.h"

#include "exception.h"
#include "config.h"
#include "connection.h"

#include "queue.h"

#include "service.h"

#include "dispatch.h"

#define szFrameClass "MdiFrame"

#define DATAVIEW_WINDOW     "DATA_VIEW_WINDOW"
#define CONSOLE_WINDOW      "CONSOLE_WINDOW"
#define DATA_RENDER_WINDOW  "DATA_RENDER_WINDOW"

#define WM_DT WM_USER+200


#include "resource/resource.h"
#include "callbacks.h"

#include <stdio.h>

#include <commctrl.h>

#define MAX_SIZE	20
#define CONNECTION_AREA_WIDTH 200
#define SPLITER_WIDTH 2

typedef struct{
    HWND hwnd;
    
    HWND toolBarHwnd;
    HWND statusBarHwnd;
    
    HWND connectionHwnd;
    HWND dataHwnd;
	HWND systemViewHwnd;
    HWND attributeHwnd;
    
    HWND westSplitHwnd;
    HWND southSplitWnd;
    
    int toolbarHeight;
    int statusbarHeight;
    
    int connectionAreaWitdh;
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

	HWND hashViewHwnd;
	HWND stringViewHwnd;
	HWND listViewHwnd;
	HWND setViewHwnd;
    HWND zsetViewHwnd;
	HWND keyNameHwnd;

	HWND visibleHwnd;

    RedisReply * data;// = (RedisReply *)wParam;
	int type;       // = (int) lParam;
}DataView;

typedef struct{
	TcpConnection * connection;

	AppView * view;

	HMENU hDev;

	HMENU hServerInfoMenu;

	HWND logHwnd;

	DataView    * dataView;

	ConsoleView * consoleView;

	/** 选中的树节点 */
	HTREEITEM selectedNode;

	int database;

    int UI;

    HINSTANCE hInstance;

    Keyspace * keyspaces;
	int spaceCount;
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



void appendLog(const char * text);

TreeNode * buildTreeNode();

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

#endif

