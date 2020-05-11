#ifndef _main_h
#define _main_h

#include <windows.h>

#include "view.h"
#include "resource.h"

#include "debug.h"

typedef struct{
	TcpClient * tcpClient;

	View * view;

	HMENU hDev;

	HWND logHwnd;

	HTREEITEM selectedNode;
}MainModel;

typedef struct{
	HWND logHwnd;

	HWND cmdHwnd;

	WNDPROC defaultCmdProc;

	MainModel * mainModel;
}ConsoleView;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK GoToLineDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);

void initpan();


void command(HWND _hwnd,int cmd);

LPTSTR mGetOpenFileName(HWND hwnd);

LPTSTR mGetSaveFileName(HWND hwnd);

#endif

