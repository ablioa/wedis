#ifndef stringview_h
#define stringview_h

#include <windows.h>

#include "main.h"

#include "resource.h"

#define STRING_VIEW_CLASS "STRING_VIEW_CLASS"

HWND buildStringViewWindow(HWND parent);

LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_stringview(HINSTANCE hInstance);

#endif
