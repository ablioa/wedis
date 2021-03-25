#ifndef stringview_h
#define stringview_h

#include "main.h"
#include "wedis.h"
#include "redis.h"

#include "resource/resource.h"

#define STRING_VIEW_CLASS "STRING_VIEW_CLASS"
#define STRING_VIEW_PROP_CLASS "STRING_VIEW_PROP_CLASS"

#define MIN_HEX_WIDTH     16
#define HEX_WIDTH_STEP    8
#define MAX_HEX_WIDTH     48

typedef enum display_mode{
	TEXT,
	BINARY
}DisplayMode;

typedef struct widget{
    HWND   hwnd;
    RECT   position;
    char * window_class;
    char * name;
    int    style;
    HMENU  id;
}widget;


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
}PropertyModel;

LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_stringview(HINSTANCE hInstance);

char * get_output_buffer(int size,int width);

char * dump_text( char * text,int len,int width);

#endif
