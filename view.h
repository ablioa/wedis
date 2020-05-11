#ifndef _view_h
#define _view_h

#define _WIN32_IE 0x0600

#include <windows.h>
#include "include/smctrl.h"
#include "resource.h"
#include "include/scintilla.h"
#include "callbacks.h"

#include <stdio.h>

#include <commctrl.h>

#define MAX_SIZE	20
#define CONNECTION_AREA_WIDTH 200
#define SPLITER_WIDTH 2

typedef struct{
	HWND hwnd;
	HWND parent;

	int  cntLeft;
	int  cntRight;

	HWND hwnLeft[20];
	HWND hwnRight[20];

	int leftSize;
	int rightSize;
}VerticalSpliter;

typedef struct{
	HWND hwnd;
	HWND parent;

	int  cntTop;
	int  cntDown;

	HWND hwnTop[20];
	HWND hwnDown[20];

	int topSize;
	int downSize;
}SouthSpliter;

typedef struct{
	HWND dataViewHwnd;
	HWND exportBtnHwnd;
	HWND saveBtnHwnd;
	HWND viewTypeHwnd;

	HWND reloadBtnHwnd;
	HWND removeBtnHwnd;
	HWND ttlBtnHwnd;
	HWND renameBtnHwnd;
	HWND keyEditHwnd;

	WNDPROC ttlBtnProc;
	WNDPROC exportBtnProc;
}DataView;

typedef struct{
	int level;
	int database;
	char * key;
}TreeNode;

TreeNode * buildTreeNode();

class View{
	private:
		HWND _hwnd;

		HWND toolBarHwnd;
	    HWND statusBarHwnd;

	    HWND connectionHwnd;
	    HWND dataHwnd;
	    HWND attributeHwnd;

	    HWND westSplitHwnd;
	    HWND southSplitWnd;

		int toolbarHeight;
	    int statusbarHeight;

	    int connectionAreaWitdh;

	    VerticalSpliter vs;
	    SouthSpliter southSpliter;

	public:
		View(HWND hwnd):_hwnd(hwnd){
		    connectionAreaWitdh = CONNECTION_AREA_WIDTH;
		}

		HWND getTreeHwnd(){return connectionHwnd;}

		void CreateView();
		void Size();

		void buildToolBar(HWND parent);
		void buildStatusBar(HWND parent);

		void initDataView(HWND _hdView);

		void buildConnectionView(HWND _hwnd);

		void buildListView(HWND parent);

		void buildDataView(HWND _hwnd);

		void buildAttributeView(HWND _hwnd);

		int getConnectionWidth();

		int getSpliterWidth(){
		    return SPLITER_WIDTH;
	    };

	    int getAttributeHeight();

		void getConnectionRect(RECT * rt,RECT * rect);

		void getDataRect(RECT * rt,RECT * rect);
		void getSpliterRect(RECT * rt,RECT * rect);

		void initVerticalSpliter(HWND parent,HWND _hwnd);
	    void addWindow(HWND hLeft,HWND hRight);
	    void setVsplitHwnd();

	    void initSouthSpliter(HWND parent,HWND _hwnd);
	    void addWindowS(HWND hLeft,HWND hRight);
	    void setVsplitHwndS();

	    void getSouthSpliterRect(RECT * rt,RECT * rect);
	    void getAttributeRect(RECT * rt,RECT * rect);
};

#endif
