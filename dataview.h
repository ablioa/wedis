#ifndef dataview_h
#define dataview_h

#include "wedis.h"
#include "main.h"
#include "redis.h"

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void switchView(HWND hwnd,int type,TreeNode * treeNode,RedisReply reply);

#endif
