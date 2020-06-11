#ifndef zsetview_h
#define zsetview_h

#include <windows.h>
//#define _WIN32_IE 0x0600
#include "main.h"

#include "redis.h"

#include "resource/resource.h"

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

#define LIST_INSERT_CMD 100
#define LIST_EXPORT_CMD 101
#define LIST_DELETE_CMD 102

#define ZSET_VIEW_CLASS "ZSET_VIEW_CLASS"

typedef struct{
   HWND zsetView;

   HWND btnInsert;

   HWND btnDelete;

   HWND btnExport;

}ZsetViewModel;

HWND buildZsetViewWindow(HWND parent);

LRESULT CALLBACK ZsetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_zsetview(HINSTANCE hInstance);

BOOL inertInto(HWND hwnd,RedisReply * reply);

#endif