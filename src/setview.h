#ifndef setview_h
#define setview_h

#include "main.h"
#include "wedis.h"
#include "resource/resource.h"


/* window styles to set data type view */
#define SET_VIEW_CLASS "SET_VIEW_CLASS"
#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

typedef struct set_view_model{
   HWND setView;

   HWND toolBar;

   RedisReply data;

   TreeNode * dataNode;
}SetViewModel;

LRESULT CALLBACK SetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void init_setview(HINSTANCE hInstance);

#endif
