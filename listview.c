#include "listview.h"

HWND listView;

char * listColNames[2]={
    "Row",
	"Value"
};

BOOL InsertListViewItems1(HWND hwnd, int cItems){
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    for (int index = 0; index < cItems; index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.pszText = "sdsd";
    
        if (ListView_InsertItem(hwnd, &lvI) == -1)
            return FALSE;
    }

    return TRUE;
}

BOOL InitListDViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 2; iCol++){
        lvc.iSubItem = iCol;
        lvc.pszText = listColNames[iCol];
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
	HINSTANCE hinst = (HINSTANCE)GetWindowLong(parent,GWL_HINSTANCE);

	RECT rect;
	GetClientRect (parent, &rect);
	
	HWND dataViewHwnd  = CreateWindowEx(0, 
		LIST_VIEW_CLASS, (""), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
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

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;

	switch(message){
	    case WM_CREATE:{
		    HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
            GetClientRect (hwnd, &rect); 

	        listView = CreateWindowEx(WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_SORTASCENDING,
                          0, 0,
                          rect.right - rect.left,
                          rect.bottom - rect.top,
                          hwnd, NULL, hinst, NULL);
            
            ListView_SetExtendedListViewStyle(listView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

			InitListDViewColumns(listView);
            InsertListViewItems1(listView,3);        
		    break;
		}

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(listView,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
		    break;
		}
        
        case WM_NOTIFY:{
			switch (((LPNMHDR) lParam)->code) {
                MessageBox(hwnd,"sss","sdsd",MB_OK);

                case LVN_GETDISPINFO:{
                     NMLVDISPINFO* plvdi;
                     plvdi = (NMLVDISPINFO*)lParam;
                 
                     switch (plvdi->item.iSubItem){
                        case 0:
                            plvdi->item.pszText = "11111";
                        break;
                            
                        case 1:
                            plvdi->item.pszText = "22222";
                        break;
                        
                        case 2:
                            plvdi->item.pszText = "33333";
                        break;
                    }  
               }
            }

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
    listViewClass.hbrBackground = CreateSolidBrush(RGB(0,0,240));
    listViewClass.lpszMenuName  = 0;
    listViewClass.lpszClassName = LIST_VIEW_CLASS;
    listViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&listViewClass);
}
