#ifndef setview_h
#define setview_h

#include <windows.h>

#include "main.h"

#include "resource.h"

#define SET_VIEW_CLASS "SET_VIEW_CLASS"

HWND buildSetViewWindow(HWND parent);

LRESULT CALLBACK SetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_setview(HINSTANCE hInstance);

#endif