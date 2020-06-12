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

#include "callbacks.h"

#include "resource/resource.h"

#include "exception.h"
#include "config.h"
#include "connection.h"

#include "queue.h"

#include "service.h"

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

char * getOutputBuffer(int size);

char * dumpText( char * text,int len);

void appendLog(char * text);

#endif

