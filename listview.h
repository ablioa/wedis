#ifndef listview_h
#define listview_h

#include <windows.h>

#include "main.h"

#include "resource.h"

#define LIST_VIEW_CLASS "LIST_VIEW_CLASS"

HWND buildListViewWindow(HWND parent);

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_listview(HINSTANCE hInstance);

#endif