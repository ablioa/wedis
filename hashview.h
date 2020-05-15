#ifndef hashview_h
#define hashview_h

#include <windows.h>

#include "main.h"

#include "resource.h"

#define HASH_VIEW_CLASS "HASH_VIEW_CLASS"

HWND buildHashViewWindow(HWND parent);

LRESULT CALLBACK HashViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_hashview(HINSTANCE hInstance);

#endif
