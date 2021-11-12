#ifndef wedis_h
#define wedis_h

#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "log.h"
#include "queue.h"
#include "redis.h"
#include "resource/resource.h"

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

#define GENERAL_CMD_RENAME  10001
#define GENERAL_CMD_SETTTL  10002
#define GENERAL_CMD_REMOVE  10003
#define GENERAL_CMD_RELOAD  10004
#define GENERAL_CMD_KEYEDIT 10005

#define WM_DT                      WM_USER+200
#define WM_DTT                     WM_USER+201
#define WM_INIT_ENTRY              WM_USER+202
#define WM_DATA_KEY                WM_USER+203
#define WM_DATA_VALUE              WM_USER+204
#define WM_NEW_CONNECTION          WM_USER+205
#define WM_UPDATE_CONNECTION_LIST  WM_USER+206

#define WM_MOVEDATA_CMD 10000

#define ERR_NORMAL					0x0000
#define ERR_CANNOT_CONNECTED		0X0001
#define ERR_CANNOT_CREATESOCKET		0x0002
#define ERR_SENDDATA_FAILED			0x0003
#define ERR_LINK_BREAKDOWN			0x0004


typedef struct app_view{
    HWND hwnd;

    HWND toolBarHwnd;
    HWND statusBarHwnd;

    HWND overviewHwnd;
    HWND dataviewHwnd;

    HWND westSplitHwnd;

	int statusbarHeight;
	int toolbarHeight;
}AppView;

typedef struct data_view{
	WNDPROC ttlBtnProc;
	WNDPROC exportBtnProc;

	HWND hashViewHwnd;
	HWND stringViewHwnd;
	HWND listViewHwnd;
	HWND setViewHwnd;
    HWND zsetViewHwnd;
	HWND systemViewHwnd;
	HWND databaseViewHwnd;

	HWND visibleHwnd;

    RedisReply data;
	int type;
}DataView;

typedef struct application{
	AppView     * view;
	DataView    * dataView;

	HMENU hConnectionMenu;

  HINSTANCE hInstance;

	HWND mainWindowHwnd;
	int spaceCount;

	TreeNode * activeHost;

	TreeNode * selectHost;

  TreeNode ** hosts;

  int totalHosts;

}Application;

extern Application * App;

typedef struct system_resource{
    HFONT      ctrlFont;
    HFONT      fixedWidthFont;
    HIMAGELIST icons;
    HBRUSH     brush;
}SystemResource;

extern SystemResource * resource;

typedef struct err_info{
	LONG ecode;
	char Messag[MAX_PATH];
	UINT Style;
}eMsg;

extern eMsg err_list[];

int DumpMessage(long Err_Code);

void initResource();

void log_message(const char * message);

#endif
