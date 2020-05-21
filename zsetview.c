#include "zsetview.h"

HWND zsetView;

char * zsetColNames[3]={
    "Row",
	"Value",
	"Score"
};

PETINFO rgPetInfo[] = 
{
    {TEXT("Dog"),  TEXT("Poodle"),     TEXT("$300.00")},
    {TEXT("Cat"),  TEXT("Siamese"),    TEXT("$100.00")},
    {TEXT("Fish"), TEXT("Angel Fish"), TEXT("$10.00")},
    {TEXT("Bird"), TEXT("Parakeet"),   TEXT("$5.00")},
    {TEXT("Toad"), TEXT("Woodhouse"),  TEXT("$0.25")},
};

/**
 * 向列表中插入数据 
 */
BOOL InsertListViewItems(HWND hwnd, int cItems){
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

/**
 * 初始化列表表头 
 */
BOOL InitZsetViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 3; iCol++){
        lvc.iSubItem = iCol;
        lvc.pszText = zsetColNames[iCol];
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

HWND buildZsetViewWindow(HWND parent){
	HINSTANCE hinst = (HINSTANCE)GetWindowLong(parent,GWL_HINSTANCE);

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

LRESULT CALLBACK ZsetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
	switch(message){
	    case WM_CREATE:{
			HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
            GetClientRect (hwnd, &rect); 
	        
            zsetView = CreateWindowEx(WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_SORTASCENDING,
                          0, 0,
                          rect.right - rect.left,
                          rect.bottom - rect.top,
                          hwnd, NULL, hinst, NULL);

			InitZsetViewColumns(zsetView);
            InsertListViewItems(zsetView,3);

			ListView_SetExtendedListViewStyle(zsetView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
		    break;
		}
		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(zsetView,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
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
    zsetViewClass.hbrBackground = CreateSolidBrush(RGB(0,240,0));
    zsetViewClass.lpszMenuName  = 0;
    zsetViewClass.lpszClassName = ZSET_VIEW_CLASS;
    zsetViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&zsetViewClass);
}
