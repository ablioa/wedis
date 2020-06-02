#ifndef _callbacks_h
#define _callbacks_h

#include <windows.h>
#include "resource/resource.h"

BOOL CALLBACK SetPreferenceProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK SetTtlDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);

BOOL CALLBACK conectionConfigDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);
#endif