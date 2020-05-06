#ifndef _control_h
#define _control_h

#include <windows.h>

#include "view.h"
#include "common.h"
#include "callbacks.h"
#include "main.h"
#include "resource.h"
#include "common.h"
#include "canvas.h"
#include "debug.h"

#include <stdio.h>


class Controller{

public:
    Controller(HWND hwnd, CREATESTRUCT * pCreate);
	void LoadSourceView();
    ~Controller ();
    void    Size (int x, int y);
    void    Paint ();
    void    Command (int cmd);
	HMENU	  hDev;

private:
    HWND      _hwnd;
    View      * _view;
	HANDLE hHeap;
	App      * app;
	HMENU     hmain;
};

BOOL CALLBACK GoToLineDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);
#endif

