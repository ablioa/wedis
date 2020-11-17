#include "hashview.h"

const char * colNames[3]={
    "Row",
	"Key",
	"Value"
};

BOOL InitHashViewColumns(HWND hWndListView) { 
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

	HINSTANCE hinst = mainModel->hInstance;
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

BOOL updateHashDataSet(HWND hwnd,RedisReply reply){
    // char indexBuff[256] = {0};
    // LVITEM lvI;

    // lvI.pszText   = LPSTR_TEXTCALLBACK;
    // lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    // lvI.stateMask = 0;
    // lvI.iSubItem  = 0;
    // lvI.state     = 0;

    // SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);
    
    // int count         = reply->bulks->count;
    // RedisBulk * bulks = reply->bulks->items;
    // for (int index = 0; index < (count/2); index++){
    //     lvI.iItem  = index;
    //     lvI.iImage = index;
    //     lvI.iSubItem = 0;

    //     memset(indexBuff,0,256);
    //     sprintf(indexBuff,"%d",(index +1));

    //     lvI.pszText = indexBuff; 
    //     ListView_InsertItem(hwnd, &lvI);

    //     lvI.pszText = bulks[index * 2]->content;
    //     lvI.iSubItem = 1;
    //     SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

    //     lvI.pszText = bulks[index *2+1]->content;
    //     lvI.iSubItem = 2;
    //     SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    // }

    return TRUE;
}


LRESULT CALLBACK HashViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    HashViewModel * hashViewModel = (HashViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
			HINSTANCE hinst = mainModel->hInstance;
            GetClientRect (hwnd, &rect); 

            hashViewModel = (HashViewModel*)malloc(sizeof(HashViewModel));
            memset(hashViewModel,0,sizeof(HashViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)hashViewModel);
	        
            hashViewModel->hashView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,
                          rect.right - rect.left - 60,
                          rect.bottom - rect.top,
                          hwnd, NULL, hinst, NULL);
            
            ListView_SetExtendedListViewStyle(hashViewModel->hashView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

			InitHashViewColumns(hashViewModel->hashView);

            // hashViewModel->btnInsert = CreateWindowEx(0, WC_BUTTON, ("Insert"), 
            // WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 0, 50, 24, hwnd, (HMENU)0, 
            // mainModel->hInstance, 0);

            // hashViewModel->btnDelete = CreateWindowEx(0, WC_BUTTON, ("Delete"), 
            // WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 29, 50, 24, hwnd, (HMENU)0, 
            // mainModel->hInstance, 0);

            // hashViewModel->btnExport = CreateWindowEx(0, WC_BUTTON, ("Export"), 
            // WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 58, 50, 24, hwnd, (HMENU)0, 
            // mainModel->hInstance, 0);

            // HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            // SendMessage(hashViewModel->btnInsert, WM_SETFONT, (WPARAM)hfont0, FALSE);
            // SendMessage(hashViewModel->btnDelete, WM_SETFONT, (WPARAM)hfont0, FALSE);
            // SendMessage(hashViewModel->btnExport, WM_SETFONT, (WPARAM)hfont0, FALSE);
		    break;
		}

        case WM_DT:{
            RedisReply rp = (RedisReply)wParam;
            updateHashDataSet(hashViewModel->hashView,rp);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(hashViewModel->hashView,0,0,rect.right-rect.left - 60,rect.bottom-rect.top,TRUE);

            // MoveWindow(hashViewModel->btnInsert,rect.right - rect.left - 55,0, 50,24,TRUE);
            // MoveWindow(hashViewModel->btnDelete,rect.right - rect.left - 55,29,50,24,TRUE);
            // MoveWindow(hashViewModel->btnExport,rect.right - rect.left - 55,58,50,24,TRUE);
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

