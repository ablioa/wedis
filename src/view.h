#ifndef view_h
#define view_h

#include "wedis.h"
#include "main.h"
#include "redis.h"

#include "entry.h"
#include "resource/resource.h"
#include <windowsx.h>

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void switchView(HWND hwnd,int type,TreeNode * treeNode,RedisReply reply);



#define STRING_VIEW_CLASS "STRING_VIEW_CLASS"
#define STRING_VIEW_PROP_CLASS "STRING_VIEW_PROP_CLASS"

#define MIN_HEX_WIDTH     16
#define HEX_WIDTH_STEP    8
#define MAX_HEX_WIDTH     48

typedef enum display_mode{
	TEXT,
	BINARY
}DisplayMode;

void create_widget(HWND parent,HINSTANCE hinst,widget * wgt);

typedef struct widget_layout{
    HWND * widgets;
    int  * width;
    int    top;
}WidgetLayout;

typedef struct string_data_model{
   HWND stringView;
   HWND propertyView;

   HWND toolBar;

   HMENU hDatabaseMenu;

   RedisReply data;

   TreeNode * dataNode;

   char * hex_buff;

   int hex_buff_size;

   DisplayMode mode;

   int hex_width;
}StringViewModel;

typedef struct data_property_model{
    HWND ttlSetButton;

    HWND ttlLabelHwnd;
    HWND ttlEditHwnd;
    HWND ttlUnitHwnd;
    HWND ttlUpdateHwnd;
    HWND ttlPersistHwnd;


   RedisReply data;

   TreeNode * dataNode;
}PropertyModel;

LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_stringview(HINSTANCE hInstance);

char * get_output_buffer(int size,int width);

char * dump_text( char * text,int len,int width);

void arrange_widgets(HWND hwnd,widget wgts[],int length);



#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

#define ZSET_VIEW_CLASS "ZSET_VIEW_CLASS"

typedef struct zset_view_model{
   HWND zsetView;
   HWND toolBar;
   
   RedisReply data;

   TreeNode * dataNode;
}ZsetViewModel;

LRESULT CALLBACK ZsetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_zsetview(HINSTANCE hInstance);

BOOL UpdateZsetData(HWND hwnd,RedisReply reply);

#define HASH_VIEW_CLASS "HASH_VIEW_CLASS"

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

typedef struct{
   HWND hashView;

   HWND toolbar;

   RedisReply data;

   TreeNode * dataNode;
}HashViewModel;

void init_hashview(HINSTANCE hInstance);

LRESULT CALLBACK HashViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

#define LIST_INSERT_CMD  100

#define LIST_VIEW_CLASS "LIST_VIEW_CLASS"

typedef struct{
   HWND listView;
   HWND toolBar;

   RedisReply data;
   
   TreeNode * dataNode;
}ListViewModel;

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_listview(HINSTANCE hInstance);

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

#define DATABASE_VIEW_CLASS "DATABASE_VIEW_CLASS"

#define DB_CTRL_CURSOR       10001
#define DB_CTRL_PATTERN      10002
#define DB_CTRL_COUNT        10003
#define DB_CTRL_SEARCH       10004
#define DB_CTRL_NEXT_SEARCH  10005


typedef struct{
   HWND toolbar;

   HWND hwndCursorText;
   HWND hwndPatternText;
   HWND hwndCountText;

   HWND hwndSearchButton;
   HWND hwndNextSearchButton;

   TreeNode * databaseNode;
}DatabaseViewModel;

void init_hashview(HINSTANCE hInstance);
void init_database_view(HINSTANCE hInstance);

BOOL CALLBACK enumChildProc(HWND hwnd,LPARAM lParam);

void create_database_view(HWND hwnd,DatabaseViewModel * model);

LRESULT CALLBACK DatabaseViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#define SYSTEM_VIEW_CLASS "SystemViewClass"

struct system_view_model{
    HWND paramViewHwnd;
    HWND toolBar;
};

typedef struct system_view_model * SystemViewModel;

LRESULT CALLBACK SystemViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_systemview(HINSTANCE hInstance);

void stat_command(HWND hwnd,int cmd);

void SetDialogIcon(HWND hWnd, int iconId);

void MoveToScreenCenter(HWND hwnd);

BOOL CALLBACK SetPreferenceProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK conectionConfigDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK ListItemEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif
