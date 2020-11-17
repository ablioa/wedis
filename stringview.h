#ifndef stringview_h
#define stringview_h

#include <windows.h>

#include "main.h"
#include "wedis.h"
#include "redis.h"

#include "resource/resource.h"

#define STRING_VIEW_CLASS "STRING_VIEW_CLASS"

typedef struct{
   HWND stringView;

   // HWND btnInsert;

   // HWND btnDelete;

   // HWND btnExport;

   HWND toolBar;

   RedisReply data;

}StringViewModel;


HWND buildStringViewWindow(HWND parent);

LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_stringview(HINSTANCE hInstance);

#endif
