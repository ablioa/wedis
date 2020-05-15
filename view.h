#ifndef _view_h
#define _view_h

#define _WIN32_IE 0x0600

#include <windows.h>
#include "resource.h"
#include "callbacks.h"

#include "main.h"

#include <stdio.h>

#include <commctrl.h>

#define MAX_SIZE	20
#define CONNECTION_AREA_WIDTH 200
#define SPLITER_WIDTH 2

TreeNode * buildTreeNode();

AppView * buildAppView(HWND hwnd);

void buildToolBar(AppView * appView);

void buildStatusBar(AppView * view);

void buildDataView(AppView * view);

void buildConnectionView(AppView * view);

void getConnectionRect(AppView * view,RECT * rt,RECT * rect);

int getConnectionWidth(AppView * view);

void buildAttributeView(AppView * view);

int getAttributeHeight(AppView * view);

void CreateView(AppView * appView);

void Size(AppView * appView);

void getDataRect(AppView * view,RECT * rt,RECT * rect);

void getSpliterRect(AppView * view,RECT * rt,RECT * rect);

void getSouthSpliterRect(AppView * view,RECT * rt,RECT * rect);

void getAttributeRect(AppView * view,RECT * rt,RECT * rect);

#endif
