#ifndef hashview_h
#define hashview_h

#include <windows.h>

#include "main.h"

#include "resource/resource.h"

#define HASH_VIEW_CLASS "HASH_VIEW_CLASS"

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

typedef struct{
   HWND hashView;
   HWND toolBar;
}HashViewModel;

HWND buildHashViewWindow(HWND parent);

LRESULT CALLBACK HashViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_hashview(HINSTANCE hInstance);

#endif
