#ifndef _databseview_h
#define _databseview_h

#include "main.h"
#include "resource/resource.h"

#define DATABASE_VIEW_CLASS "DATABASE_VIEW_CLASS"

typedef struct{
   HWND toolBar;
}DatabaseViewModel;

void init_hashview(HINSTANCE hInstance);

HWND buildDatabaseToolBar(HWND parent);

LRESULT CALLBACK DatabaseViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif
