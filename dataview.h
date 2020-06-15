#ifndef dataview_h
#define dataview_h

#include "wedis.h"
#include "main.h"
#include "redis.h"

// #define	REPLY_STATUS  0
// #define	REPLY_ERROR   1
// #define	REPLY_DIGITAL 2
// #define	REPLY_BULK    3
// #define	REPLY_MULTI   4

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK dataRenderProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void switchView(HWND hwnd,int type,RedisReply * reply);

#endif
