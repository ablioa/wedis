#include "listview.h"

const char * listColNames[2]={
    "Row",
	"Value"
};

HWND buildListToolBar(HWND parent){
	HINSTANCE hInst = mainModel->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 4;
	TBBUTTON tbtn[4] = {
        {(0), IDM_CONNECTION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
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

BOOL InitListDViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;
	char * valName;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 2; iCol++){
        lvc.iSubItem = iCol;

		// 注意内存释放
		valName = (char *) malloc(sizeof(char) * 128);
		memset(valName,0,sizeof(char) * 128);
		strcpy(valName,listColNames[iCol]);

        lvc.pszText = valName;
        lvc.cx = 100;

        if ( iCol < 2 ){
            lvc.fmt = LVCFMT_LEFT;
		}
        else{
            lvc.fmt = LVCFMT_RIGHT;
		}

        if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1){
            return FALSE;
		}
    }
    
    return TRUE;
}

HWND buildListViewWindow(HWND parent){
    RECT rect;
	GetClientRect (parent, &rect);
	
	HWND dataViewHwnd  = CreateWindowEx(0, 
		LIST_VIEW_CLASS, (""), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
        0, 
		0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        parent, 
	    (HMENU)0, 
		mainModel->hInstance, 
		0);

	ShowWindow(dataViewHwnd,SW_HIDE);

	return dataViewHwnd;
    
}

BOOL updateListDataSet(HWND hwnd,RedisReply reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);
    for(int ix = 0; ix < reply->array_length; ix ++){
        RedisReply item = reply->bulks[ix];
        
        lvI.iItem  = ix;
        lvI.iImage = ix;
        lvI.iSubItem = 0;

        memset(indexBuff,0,256);
        sprintf(indexBuff,"%d",(ix +1));

        lvI.pszText = indexBuff; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = item->bulk->content;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
	ListViewModel * listViewModel = (ListViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
	    case WM_CREATE:{
            listViewModel = (ListViewModel*)malloc(sizeof(ListViewModel));
            memset(listViewModel,0,sizeof(ListViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)listViewModel);

            GetClientRect (hwnd, &rect);

	        listViewModel->listView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_VISIBLE| WS_BORDER | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,
                          rect.right - rect.left -60,
                          rect.bottom - rect.top,
                          hwnd, NULL, mainModel->hInstance, NULL);

            listViewModel->toolBar = buildListToolBar(hwnd);
            
            ListView_SetExtendedListViewStyle(listViewModel->listView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

			InitListDViewColumns(listViewModel->listView);

		    break;
		}

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
                case LVN_COLUMNCLICK:{
                    break;
                }
                case NM_DBLCLK:{
                    DialogBox(mainModel->hInstance,MAKEINTRESOURCE (IDD_LIST_ITEM),hwnd,(DLGPROC)ListItemEditProc);
                    break;
                }
            }
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD (wParam)){
                case LIST_INSERT_CMD:{
                    MessageBox(hwnd,"insert list item right now!","title",MB_OK);
                    break;
                }

                case LIST_EXPORT_CMD:{
//                    char * filename = mGetOpenFileName(hwnd);
                    break;
                }

                case LIST_DELETE_CMD:{
                    MessageBox(hwnd,"delete list item right now!","title",MB_OK);
                    break;
                }
            }
            break;
        }

        case WM_DT:{
            RedisReply rp = (RedisReply)wParam;
            updateListDataSet(listViewModel->listView,rp);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
            MoveWindow(listViewModel->toolBar,0,0,rect.right-rect.left,28,TRUE);
			MoveWindow(listViewModel->listView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

void init_listview(HINSTANCE hInstance){
	WNDCLASSEX listViewClass;

    listViewClass.cbSize        = sizeof(WNDCLASSEX);
    listViewClass.style         = 0;
    listViewClass.lpfnWndProc   = ListViewWndProc;
    listViewClass.cbClsExtra    = 0;
    listViewClass.cbWndExtra    = 0;
    listViewClass.hInstance     = hInstance;
    listViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    listViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    listViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    listViewClass.lpszMenuName  = 0;
    listViewClass.lpszClassName = LIST_VIEW_CLASS;
    listViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    
    RegisterClassEx(&listViewClass);
}
