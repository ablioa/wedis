#ifndef _entry_h
#define _entry_h

#include "resource/resource.h"

#define WDS_PANE "window.container"

BOOL CALLBACK entryDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

void init_editor_view(HINSTANCE hInstance);

#endif
