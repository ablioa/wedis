#ifndef _databseview_h
#define _databseview_h

#include "main.h"
#include "resource/resource.h"

#define DATABASE_VIEW_CLASS "DATABASE_VIEW_CLASS"

#define DB_CTRL_CURSOR       10001
#define DB_CTRL_PATTERN      10002
#define DB_CTRL_COUNT        10003
#define DB_CTRL_SEARCH       10004
#define DB_CTRL_NEXT_SEARCH  10005


typedef struct{
   HWND toolBar;

   HWND hwndCursorText;
   HWND hwndPatternText;
   HWND hwndCountText;

   HWND hwndSearchButton;
   HWND hwndNextSearchButton;

   TreeNode * databaseNode;
}DatabaseViewModel;

void init_hashview(HINSTANCE hInstance);

BOOL CALLBACK enumChildProc(HWND hwnd,LPARAM lParam);

void create_database_view(HWND hwnd,DatabaseViewModel * model);

HWND buildDatabaseToolBar(HWND parent);

LRESULT CALLBACK DatabaseViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif
