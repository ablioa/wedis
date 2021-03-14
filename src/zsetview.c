#include "zsetview.h"

const char * zsetColNames[3]={
    "Row",
	"Value",
	"Score"
};

BOOL InitZsetViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int iCol = 0; iCol < 3; iCol++){
        char * buff = (char*)calloc(1,255);
        strcpy(buff,zsetColNames[iCol]);

        lvc.iSubItem = iCol;
        lvc.pszText = buff;
        lvc.cx = 100;
        lvc.fmt = LVCFMT_LEFT;
        ListView_InsertColumn(hWndListView, iCol, &lvc);
    }
    
    return TRUE;
}

/** TODO make it a universal function. */
HWND buildGeneralToolBar(HWND parent,TBBUTTON * tbtn,int buttonCount){
	HINSTANCE hInst = App->hInstance;
	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_ZSETTB));
    HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, buttonCount);
	ImageList_AddMasked(hImageList, hBmp, RGB(255,255,255));

	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;
    HWND tb = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle, 16, 16, 16, 16, parent, (HMENU) 0, hInst, NULL);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) hImageList);
    SendMessage(tb, TB_ADDBUTTONS,  (WPARAM)buttonCount,(LPARAM)tbtn);
    SendMessage(tb, TB_AUTOSIZE, 0, 0);

    ShowWindow(tb,  TRUE);
	return tb;
}


HWND buildZsetToolBar(HWND parent){
    int buttonCount = 1;
	TBBUTTON tbtn[1] = {
        {(0), ZSET_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

	return buildGeneralToolBar(parent,tbtn,buttonCount);
}

LRESULT CALLBACK ZsetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    ZsetViewModel * zsetViewModel = (ZsetViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
	switch(message){
	    case WM_CREATE:{
            zsetViewModel = (ZsetViewModel*)calloc(1,sizeof(ZsetViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)zsetViewModel);

			HINSTANCE hinst = App->hInstance;
            GetClientRect (hwnd, &rect); 
	        
            zsetViewModel->zsetView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, hinst, NULL);

            zsetViewModel->toolBar = buildZsetToolBar(hwnd);
			InitZsetViewColumns(zsetViewModel->zsetView);
            
			ListView_SetExtendedListViewStyle(zsetViewModel->zsetView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
		    break;
		}

	    case WM_COMMAND:{
			switch(LOWORD(wParam)){
			    case ZSET_DELETE_DATA:{
					MessageBox(hwnd,"delete zset data.","title",MB_OK);
					break;
				}
			}
		}

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
            MoveWindow(zsetViewModel->toolBar,0,0,rect.right-rect.left,28,TRUE);
			MoveWindow(zsetViewModel->zsetView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);

		    break;
		}

        case WM_DT:{
            UpdateZsetData(zsetViewModel->zsetView,(RedisReply)wParam);
            break;
        }
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

BOOL UpdateZsetData(HWND hwnd,RedisReply reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);

    for (int index = 0; index < (reply->array_length / 2); index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        memset(indexBuff,0,256);
        sprintf(indexBuff,"%d",(index +1));

        lvI.pszText = indexBuff; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = reply->bulks[index * 2]->bulk->content;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        lvI.pszText = reply->bulks[index *2+1]->bulk->content;
        lvI.iSubItem = 2;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

void init_zsetview(HINSTANCE hInstance){
	WNDCLASSEX zsetViewClass;

    zsetViewClass.cbSize        = sizeof(WNDCLASSEX);
    zsetViewClass.style         = 0;
    zsetViewClass.lpfnWndProc   = ZsetViewWndProc;
    zsetViewClass.cbClsExtra    = 0;
    zsetViewClass.cbWndExtra    = 0;
    zsetViewClass.hInstance     = hInstance;
    zsetViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    zsetViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    zsetViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    zsetViewClass.lpszMenuName  = 0;
    zsetViewClass.lpszClassName = ZSET_VIEW_CLASS;
    zsetViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&zsetViewClass);
}
