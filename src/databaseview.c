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

void create_database_view(HWND hwnd,DatabaseViewModel * model){
    HINSTANCE hinst = App->hInstance;

    model->hwndCursorText = CreateWindowEx(0, WC_EDIT, ("0"), 
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
			5, 28, 80, 20, hwnd, (HMENU)DB_CTRL_CURSOR, hinst, 0); 
	
	model->hwndPatternText = CreateWindowEx(0, WC_EDIT, ("*"), 
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
			90, 28, 80, 20, hwnd, (HMENU)DB_CTRL_PATTERN, hinst, 0);    

    model->hwndCountText = CreateWindowEx(0, WC_EDIT, ("10"), 
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
			175, 28, 80, 20, hwnd, (HMENU)DB_CTRL_COUNT, hinst, 0); 

    model->hwndSearchButton = CreateWindowEx(0, WC_BUTTON, ("Scan"), 
			WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
			260, 28, 60, 20, hwnd, (HMENU)DB_CTRL_SEARCH, hinst, 0); 

    model->hwndNextSearchButton = CreateWindowEx(0, WC_BUTTON, ("Next"), 
			WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
			325, 28, 60, 20, hwnd, (HMENU)DB_CTRL_NEXT_SEARCH, hinst, 0); 

    SendMessage(model->hwndCursorText, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
    SendMessage(model->hwndPatternText, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
    SendMessage(model->hwndCountText, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
    SendMessage(model->hwndSearchButton, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
    SendMessage(model->hwndNextSearchButton, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
}

LRESULT CALLBACK DatabaseViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
	DatabaseViewModel * model = (DatabaseViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            model = (DatabaseViewModel*)calloc(1,sizeof(DatabaseViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);
            model->toolBar = buildDatabaseToolBar(hwnd);

			create_database_view(hwnd,model);
		    break;
		}

		case WM_COMMAND:{
			BOOL result1 = TRUE;
			BOOL result2 = TRUE;
			switch(LOWORD(wParam)){
				case DB_CTRL_NEXT_SEARCH:{
					TreeNode * node = model->databaseNode;
					node->database->cursor = GetDlgItemInt(hwnd,DB_CTRL_CURSOR,&result1,FALSE);
					node->database->page_size = GetDlgItemInt(hwnd,DB_CTRL_COUNT,&result2,FALSE);
					GetDlgItemText(hwnd,DB_CTRL_PATTERN,node->database->pattern,255);

					if(result1 && result2){
						node->database->last = node->database->cursor;
						s_db_get_data(model->databaseNode,
								node->database->cursor,
								node->database->pattern,
								node->database->page_size);
					}

					break;
				}

				case DB_CTRL_SEARCH:{
					TreeNode * node = model->databaseNode;
					node->database->cursor = GetDlgItemInt(hwnd,DB_CTRL_CURSOR,&result1,FALSE);
					node->database->page_size = GetDlgItemInt(hwnd,DB_CTRL_COUNT,&result2,FALSE);
					GetDlgItemText(hwnd,DB_CTRL_PATTERN,node->database->pattern,255);

					if(result1 && result2){
						s_db_get_data(model->databaseNode,
								node->database->last,
								node->database->pattern,
								node->database->page_size);
					}

					/*MessageBox(hwnd,"next search button","title",MB_OK);*/
					break;
				}
			}
			break;
		}

		case WM_DT:{
			RedisReply data     = (RedisReply) wParam;
            TreeNode * datanode = (TreeNode *) lParam;
			model->databaseNode = datanode;
			
			SetDlgItemText(hwnd,DB_CTRL_PATTERN,datanode->database->pattern);
			SetDlgItemInt(hwnd,DB_CTRL_CURSOR,datanode->database->cursor,FALSE);
			SetDlgItemInt(hwnd,DB_CTRL_COUNT,datanode->database->page_size,FALSE);
			break;
		}

		case WM_DTT:{
            TreeNode * datanode = (TreeNode *) wParam;
			SetDlgItemInt(hwnd,DB_CTRL_CURSOR,datanode->database->cursor,FALSE);
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
