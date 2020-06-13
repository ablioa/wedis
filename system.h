#ifndef _system_h
#define _system_h

#include <windows.h>
#include "main.h"
#include "redis.h"

#define SYSTEM_VIEW_CLASS "SystemViewClass"

LRESULT CALLBACK SystemViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void init_systemview(HINSTANCE hInstance);

#endif