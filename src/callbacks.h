#ifndef _callbacks_h
#define _callbacks_h

#include "main.h"
#include "resource/resource.h"

void SetDialogIcon(HWND hWnd, int idi);

void MoveToScreenCenter(HWND hwnd);

BOOL CALLBACK SetPreferenceProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK conectionConfigDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK ListItemEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif
