#ifndef dataview_h
#define dataview_h

#include "wedis.h"
#include "main.h"
#include "redis.h"

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK dataRenderProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void resizeDataViewWinwdow(HWND dataHwnd);

void createDataViewWindow(HWND dataHwnd);

void switchView(HWND hwnd,int type,RedisReply reply);

#endif
