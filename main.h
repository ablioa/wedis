#ifndef _main_h
#define _main_h
//------------------------------------
//  main.h
//  Generic Windows Program
//  (c) Reliable Software, 1997, 98
//------------------------------------

#include <windows.h>

#include "control.h"
#include "resource.h"

#include "debug.h"

LRESULT CALLBACK MainWndProc
   (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK AboutDlgProc
   (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif

