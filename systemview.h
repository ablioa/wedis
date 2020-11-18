#ifndef _system_h
#define _system_h

#include <windows.h>
#include "main.h"
#include "redis.h"

#define SYSTEM_VIEW_CLASS "SystemViewClass"

struct system_view_model{
    HWND paramViewHwnd;
    HWND toolBar;
};

typedef struct system_view_model * SystemViewModel;

LRESULT CALLBACK SystemViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_systemview(HINSTANCE hInstance);

#endif