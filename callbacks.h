#ifndef _callbacks_h
#define _callbacks_h

#include <windows.h>
#include "resource.h"

extern int tmp_break;
extern int dest_line;


LRESULT CALLBACK ColorProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK SetBrkProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

#endif