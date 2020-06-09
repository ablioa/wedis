#include "hashview.h"

HWND tableView;

const char * colNames[3]={
    "Row",
	"Key",
	"Value"
};

BOOL InitListViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;
	char * valName;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 3; iCol++){
		valName = (char *) malloc(sizeof(char) * 128);
		memset(valName,0,sizeof(char) * 128);
		strcpy(valName,colNames[iCol]);

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

HWND buildHashViewWindow(HWND parent){
	RECT rect;

	HINSTANCE hinst = (HINSTANCE)GetWindowLong(parent,GWLP_HINSTANCE);
	GetClientRect (parent, &rect);
	
	HWND dataViewHwnd  = CreateWindowEx(0, 
		HASH_VIEW_CLASS, (""), 
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

BOOL updateHashDataSet(HWND hwnd,RedisReply * reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);
    
    for (int index = 0; index < (reply->bulkSize/2); index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        memset(indexBuff,0,256);
        sprintf(indexBuff,"%d",(index +1));

        lvI.pszText = indexBuff; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = reply->bulks[index * 2];
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        lvI.pszText = reply->bulks[index *2+1];
        lvI.iSubItem = 2;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

LRESULT CALLBACK HashViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;

	switch(message){
	    case WM_CREATE:{
			HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd,GWLP_HINSTANCE);
            GetClientRect (hwnd, &rect); 
	        
            tableView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,
                          rect.right - rect.left - 60,
                          rect.bottom - rect.top,
                          hwnd, NULL, hinst, NULL);
            
            ListView_SetExtendedListViewStyle(tableView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

			InitListViewColumns(tableView);

                        HWND btnInsert = CreateWindowEx(0, WC_BUTTON, ("Insert"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 0, 50, 24, hwnd, (HMENU)0, 
            mainModel->hInstance, 0);

            HWND btnDelete = CreateWindowEx(0, WC_BUTTON, ("Delete"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 29, 50, 24, hwnd, (HMENU)0, 
            mainModel->hInstance, 0);

            HWND btnExport = CreateWindowEx(0, WC_BUTTON, ("Export"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 58, 50, 24, hwnd, (HMENU)0, 
            mainModel->hInstance, 0);

            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(btnInsert, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(btnDelete, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(btnExport, WM_SETFONT, (WPARAM)hfont0, FALSE);
		    break;
		}

        case WM_DT:{
            RedisReply * rp = (RedisReply *)wParam;
            updateHashDataSet(tableView,rp);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(tableView,0,0,rect.right-rect.left - 60,rect.bottom-rect.top,TRUE);
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

void init_hashview(HINSTANCE hInstance){
	WNDCLASSEX hashViewClass;

    hashViewClass.cbSize        = sizeof(WNDCLASSEX);
    hashViewClass.style         = 0;
    hashViewClass.lpfnWndProc   = HashViewWndProc;
    hashViewClass.cbClsExtra    = 0;
    hashViewClass.cbWndExtra    = 0;
    hashViewClass.hInstance     = hInstance;
    hashViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    hashViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    hashViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    hashViewClass.lpszMenuName  = 0;
    hashViewClass.lpszClassName = HASH_VIEW_CLASS;
    hashViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    
    RegisterClassEx(&hashViewClass);
}

