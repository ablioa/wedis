#include "databaseview.h"

void init_database_view(HINSTANCE hInstance){
	WNDCLASSEX hashViewClass;

    hashViewClass.cbSize        = sizeof(WNDCLASSEX);
    hashViewClass.style         = 0;
    hashViewClass.lpfnWndProc   = DatabaseViewWndProc;
    hashViewClass.cbClsExtra    = 0;
    hashViewClass.cbWndExtra    = 0;
    hashViewClass.hInstance     = hInstance;
    hashViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    hashViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    hashViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    hashViewClass.lpszMenuName  = 0;
    hashViewClass.lpszClassName = DATABASE_VIEW_CLASS;
    hashViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    
    RegisterClassEx(&hashViewClass);
}

LRESULT CALLBACK DatabaseViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
	DatabaseViewModel * model = (DatabaseViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            model = (DatabaseViewModel*)calloc(1,sizeof(DatabaseViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            model->toolBar = buildDatabaseToolBar(hwnd);

            /////

            HINSTANCE hinst = App->hInstance;


            HWND keyEditHwnd  = CreateWindowEx(0, WC_EDIT, ("scan 11 asas asasas"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 5, 28, 240, 24, hwnd, (HMENU)0, hinst, 0);    
            HWND renameBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("Rename"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 250, 28, 60, 24, hwnd, (HMENU)0, hinst, 0);     
            HWND ttlBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("TTL"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 315, 28, 60, 24, hwnd, (HMENU)0, hinst, 0);
            HWND removeBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("Remove"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 380, 28, 60, 24, hwnd, (HMENU)0, hinst, 0);
            HWND reloadBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Reload"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 445, 28, 60, 24, hwnd, (HMENU)0, hinst, 0);

            SendMessage(keyEditHwnd, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            SendMessage(renameBtnHwnd, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            SendMessage(ttlBtnHwnd, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            SendMessage(removeBtnHwnd, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            SendMessage(reloadBtnHwnd, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            /////
		    break;
		}

		case WM_SIZE:{
            GetClientRect(hwnd,&rect);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

HWND buildDatabaseToolBar(HWND parent){
	HINSTANCE hInst = App->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 4;
	TBBUTTON tbtn[4] = {
        {(0), 1, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(1), IDM_PREFERENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(2), IDM_PREFERENCE+1, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_PREFERENCE+2, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_LISTTB));
    HIMAGELIST hIcons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, buttonCount);
	ImageList_AddMasked(hIcons, hBmp, RGB(255,255,255));

    HWND tb = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle, 16, 16, 16, 16, parent, (HMENU) 0, hInst, NULL);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) hIcons);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_ADDBUTTONS,       (WPARAM)buttonCount,       (LPARAM)&tbtn);
    SendMessage(tb, TB_AUTOSIZE, 0, 0);

    ShowWindow(tb,  TRUE);

    return tb;
}