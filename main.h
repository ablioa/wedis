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

#include "entry.h"

#include "test.h"

#include "split.h"

#include "myconnection.h"


#define szFrameClass "MdiFrame"

#define DATAVIEW_WINDOW     "DATA_VIEW_WINDOW"
#define CONSOLE_WINDOW      "CONSOLE_WINDOW"
#define DATA_RENDER_WINDOW  "DATA_RENDER_WINDOW"

#define WM_DT WM_USER+200


#include "resource/resource.h"
#include "callbacks.h"

#include <stdio.h>

#include <commctrl.h>

#include "myconnection.h"
#include "mservice.h"

#define MAX_SIZE	20
#define CONNECTION_AREA_WIDTH 200
#define SPLITER_WIDTH 2

#define IDM_TIMING              1000
#define IDM_SYSTEM_STAT         1001
#define IDM_ADD                 1002
#define IDM_DEBUG_GET_DATABASES 1003
#define IDM_RELOAD              1004
#define IDM_RENAME              1005
#define IDM_REMOVE              1006
#define IDM_CONNECTION          1007
#define IDC_STRING_VIEW_TEXT    1008
#define IDC_STRING_VIEW_SAVE    1009
#define IDC_TTL_OK              1010
#define IDC_TTL_EDIT            1011
#define IDC_TTL_CANCEL          1012
#define IDC_ABOUT_IMAGE         1013
#define IDC_ABOUT_NAME          1014
#define IDC_ABOUT_URL           1015
#define IDC_ABOUT_DESCRIPTION   1016

#define IDM_CUSTOMER_HOST 900


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

    RedisReply data;// = (RedisReply *)wParam;
	int type;       // = (int) lParam;
}DataView;

typedef struct{
//	TcpConnection * connection;

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

	HWND mainWindowHwnd;

	int spaceCount;

	HTREEITEM root;
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

