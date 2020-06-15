#include "zsetview.h"

const char * zsetColNames[3]={
    "Row",
	"Value",
	"Score"
};

BOOL InitZsetViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;

	char * valName;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 3; iCol++){
		valName = (char *) calloc(128,sizeof(char));
		strcpy(valName,zsetColNames[iCol]);

        lvc.iSubItem = iCol;
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

LRESULT CALLBACK ZsetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    ZsetViewModel * zsetViewModel = (ZsetViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
	switch(message){
	    case WM_CREATE:{
            zsetViewModel = (ZsetViewModel*)calloc(1,sizeof(ZsetViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)zsetViewModel);

			HINSTANCE hinst = mainModel->hInstance;
            GetClientRect (hwnd, &rect); 
	        
            zsetViewModel->zsetView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,
                          rect.right - rect.left - 60,
                          rect.bottom - rect.top,
                          hwnd, NULL, hinst, NULL);

			InitZsetViewColumns(zsetViewModel->zsetView);
            
			ListView_SetExtendedListViewStyle(zsetViewModel->zsetView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

            zsetViewModel->btnInsert = CreateWindowEx(0, WC_BUTTON, ("Insert"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 0, 50, 24, hwnd, (HMENU)LIST_INSERT_CMD, 
            mainModel->hInstance, 0);

            zsetViewModel->btnDelete = CreateWindowEx(0, WC_BUTTON, ("Delete"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 29, 50, 24, hwnd, (HMENU)LIST_DELETE_CMD, 
            mainModel->hInstance, 0);

            zsetViewModel->btnExport = CreateWindowEx(0, WC_BUTTON, ("Export"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 58, 50, 24, hwnd, (HMENU)LIST_EXPORT_CMD, 
            mainModel->hInstance, 0);

            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(zsetViewModel->btnInsert, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(zsetViewModel->btnDelete, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(zsetViewModel->btnExport, WM_SETFONT, (WPARAM)hfont0, FALSE);
		    break;
		}

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(zsetViewModel->zsetView,0,0,rect.right-rect.left-60,rect.bottom-rect.top,TRUE);

            MoveWindow(zsetViewModel->btnInsert,rect.right - rect.left - 55,0, 50,24,TRUE);
            MoveWindow(zsetViewModel->btnDelete,rect.right - rect.left - 55,29,50,24,TRUE);
            MoveWindow(zsetViewModel->btnExport,rect.right - rect.left - 55,58,50,24,TRUE);
		    break;
		}

        case WM_DT:{
            RedisReply * rp = (RedisReply *)wParam;
            inertInto(zsetViewModel->zsetView,rp);
            break;
        }
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

BOOL inertInto(HWND hwnd,RedisReply * reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);
    
    // for (int index = 0; index < (reply->bulkSize/2); index++){
    //     lvI.iItem  = index;
    //     lvI.iImage = index;
    //     lvI.iSubItem = 0;

    //     memset(indexBuff,0,256);
    //     sprintf(indexBuff,"%d",(index +1));

    //     lvI.pszText = indexBuff; 
    //     ListView_InsertItem(hwnd, &lvI);

    //     lvI.pszText = reply->bulks[index * 2];
    //     lvI.iSubItem = 1;
    //     SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

    //     lvI.pszText = reply->bulks[index *2+1];
    //     lvI.iSubItem = 2;
    //     SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    // }

    return TRUE;
}

HWND buildZsetViewWindow(HWND parent){
	HINSTANCE hinst = mainModel->hInstance;

	RECT containerRect;
	GetClientRect (parent, &containerRect);
	
	HWND dataViewHwnd  = CreateWindowEx(0, 
		ZSET_VIEW_CLASS, (""), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
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
