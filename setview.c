#include "setview.h"

HWND buildSetViewWindow(HWND parent){
	HINSTANCE hinst = (HINSTANCE)GetWindowLong(parent,GWL_HINSTANCE);

	RECT containerRect;
	GetClientRect (parent, &containerRect);
	
	HWND dataViewHwnd  = CreateWindowEx(0, 
		SET_VIEW_CLASS, (""), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
        0, 
		0,
        containerRect.right - containerRect.left,
        containerRect.bottom - containerRect.top,
        parent, 
	    (HMENU)0, 
		hinst, 
		0);

	ShowWindow(dataViewHwnd,SW_HIDE);

	return dataViewHwnd;
    
}

LRESULT CALLBACK SetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
	    case WM_CREATE:{
	        
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

void init_setview(HINSTANCE hInstance){
	WNDCLASSEX setViewClass;

    setViewClass.cbSize        = sizeof(WNDCLASSEX);
    setViewClass.style         = 0;
    setViewClass.lpfnWndProc   = SetViewWndProc;
    setViewClass.cbClsExtra    = 0;
    setViewClass.cbWndExtra    = 0;
    setViewClass.hInstance     = hInstance;
    setViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    setViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    setViewClass.hbrBackground = CreateSolidBrush(RGB(240,0,0));
    setViewClass.lpszMenuName  = 0;
    setViewClass.lpszClassName = SET_VIEW_CLASS;
    setViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&setViewClass);
}
