#ifndef split_h
#define split_h

#include <windows.h>

#include <stdlib.h>
#include <string.h>

#define V_SPLIT "V_SPLIT"
#define H_SPLIT "H_SPLIT"

#define SPLIT_MODE_DRAGING  0
#define SPLIT_MODE_DRAGDONE 1
#define SPLIT_HANDLE_MAX    20

#define WM_BASE					0
#define WM_SET_PANES_HWND		WM_USER + WM_BASE 
#define WM_SET_BORDERS_MIN		WM_USER + WM_BASE + 1
#define WM_SET_BORDERS_COL		WM_USER + WM_BASE + 2

extern char szCtrlAppName[];

typedef struct{
    HWND parent;

    char className[255];

    int  drageMode;

	int topSize;
	int downSize;

	int leftSize;
	int rightSize;

    HWND left[SPLIT_HANDLE_MAX];
    HWND right[SPLIT_HANDLE_MAX];

    HWND top[SPLIT_HANDLE_MAX];
    HWND down[SPLIT_HANDLE_MAX];
}SplitModel;

void initSplit(HINSTANCE hInstance);

LRESULT CALLBACK hSplitProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK vSplitProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//void WINAPI DrawXorBar(HDC hdc, int x1, int y1, int width, int height);

#endif