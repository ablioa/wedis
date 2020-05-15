#ifndef dataview_h
#define dataview_h

#include "main.h"
#include "redis.h"

BOOL CALLBACK exportBtnProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK ttlBtnProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK dataRenderProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void switchView(HWND hwnd,int type,RedisReply * reply);

#endif
