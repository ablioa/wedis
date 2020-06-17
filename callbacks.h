#ifndef _callbacks_h
#define _callbacks_h

#include <windows.h>
#include "resource/resource.h"

#include "main.h"
#include "wedis.h"

void SetDialogIcon(HWND hWnd, int idi);

void MoveToScreenCenter(HWND hwnd);

BOOL CALLBACK SetPreferenceProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SetTtlDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK conectionConfigDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK ListItemEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif