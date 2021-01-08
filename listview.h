#ifndef listview_h
#define listview_h

#include <windows.h>

#include "main.h"

#include "resource/resource.h"

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

#define LIST_INSERT_CMD 100
#define LIST_EXPORT_CMD 101
#define LIST_DELETE_CMD 102

#define LIST_VIEW_CLASS "LIST_VIEW_CLASS"

typedef struct{
   HWND listView;
   HWND toolBar;

   RedisReply data;
}ListViewModel;

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_listview(HINSTANCE hInstance);

#endif