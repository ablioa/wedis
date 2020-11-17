#ifndef console_h
#define console_h

#include "windows.h"

#include "main.h"

// LRESULT CALLBACK consoleWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CmdBoxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

#endif