#ifndef zsetview_h
#define zsetview_h

#include <windows.h>

#include "main.h"

#include "resource.h"

typedef struct 
{
    TCHAR szKind[10];
    TCHAR szBreed[50];
    TCHAR szPrice[20];
}PETINFO;

extern PETINFO rgPetInfo[];

#define ZSET_VIEW_CLASS "ZSET_VIEW_CLASS"

HWND buildZsetViewWindow(HWND parent);

LRESULT CALLBACK ZsetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_zsetview(HINSTANCE hInstance);

#endif