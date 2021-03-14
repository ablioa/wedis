#ifndef stringview_h
#define stringview_h

#include "main.h"
#include "wedis.h"
#include "redis.h"

#include "resource/resource.h"

#define STRING_VIEW_CLASS "STRING_VIEW_CLASS"

typedef enum display_mode{
	TEXT,
	BINARY
}DisplayMode;

typedef struct string_data_model{
   HWND stringView;

   HWND toolBar;

   HMENU hDatabaseMenu;

   RedisReply data;

   TreeNode * dataNode;

   char * hex_buff;

   int hex_buff_size;

   DisplayMode mode;
   
}StringViewModel;


LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_stringview(HINSTANCE hInstance);

char * get_output_buffer(int size);

char * dump_text( char * text,int len);

#endif
