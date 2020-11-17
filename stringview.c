#include "stringview.h"

HWND buildStringToolBar(HWND parent){
	HINSTANCE hInst = mainModel->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 10;
	TBBUTTON tbtn[10] = {
        {(0), IDM_CONNECTION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(1), IDM_PREFERENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
		{(7), IDM_SYSTEM_STAT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(8), IDM_DEBUG_GET_DATABASES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(0), 0             , 0,               TBSTYLE_SEP,    {0}, 0, 0},
		{(2), IDM_ADD       , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_REMOVE    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(4), IDM_RELOAD    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
		{(5), IDM_RENAME    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(6), IDM_TIMING    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_MAIN));
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

HWND buildStringViewWindow(HWND parent){
	RECT rect;
	HINSTANCE hinst = mainModel->hInstance;

	GetClientRect (parent, &rect);
	HWND dataViewHwnd  = CreateWindowEx(0,
		STRING_VIEW_CLASS, (""), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
        0, 
		0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        parent, 
	    (HMENU)0, 
		hinst, 
		0);

	ShowWindow(dataViewHwnd,SW_HIDE);

	return dataViewHwnd;
}

LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    StringViewModel * model = (StringViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            model = (StringViewModel*)malloc(sizeof(StringViewModel));
            memset(model,0,sizeof(StringViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

			model->stringView = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | ES_AUTOVSCROLL | WS_BORDER | WS_CHILD | WS_TABSTOP | WS_VSCROLL |ES_MULTILINE, 0, 0, 0, 0, hwnd, (HMENU)IDC_STRING_VIEW_TEXT, mainModel->hInstance, 0);
            model->toolBar = buildStringToolBar(hwnd);
            SendMessage(model->stringView, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            // model->btnInsert  = CreateWindowEx(0, WC_BUTTON, ("Save"), WEDIS_PUSH_BUTTON_STYLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_STRING_VIEW_SAVE, mainModel->hInstance, 0);

            // SendMessage(model->btnInsert, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            // SendMessage(model->stringView, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
            
		    break;
		}
       
        case WM_COMMAND:{
            switch(LOWORD (wParam)){
                case IDC_STRING_VIEW_SAVE:{
                    // TCHAR name[256]={0};
                    // GetDlgItemText(hwnd,IDC_STRING_VIEW_TEXT,name,sizeof(name));

                    // TCHAR cmd[256];
                    // sprintf(cmd,"set %s \"%s\"",model->data->key,name);

                    // sendRedisCommand(cmd,NULL,NULL,CMD_AUTH);
                    break;
                }
            }
            break;
        }

		case WM_DT:{
            RedisReply data = (RedisReply)wParam;

            model->data = data;

			SendMessage(model->stringView,EM_REPLACESEL,FALSE,(LPARAM)data->bulk->content);
			SetWindowText(model->stringView,data->bulk->content);

            //MessageBox(hwnd,data->bulk->content,"text",MB_OK);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(model->stringView,0,28,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            // MoveWindow(model->toolBar,rect.right-rect.left - 28,0,28,rect.bottom-rect.top,TRUE);
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

void init_stringview(HINSTANCE hInstance){
	WNDCLASSEX stringViewClass;

    stringViewClass.cbSize        = sizeof(WNDCLASSEX);
    stringViewClass.style         = 0;
    stringViewClass.lpfnWndProc   = StringViewWndProc;
    stringViewClass.cbClsExtra    = 0;
    stringViewClass.cbWndExtra    = 0;
    stringViewClass.hInstance     = hInstance;
    stringViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    stringViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    stringViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    stringViewClass.lpszMenuName  = 0;
    stringViewClass.lpszClassName = STRING_VIEW_CLASS;
    stringViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&stringViewClass);
}
