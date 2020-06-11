#include "setview.h"

//HWND setView;

const char * setColNames[2]={
    "Row",
	"Value"
};

BOOL InitSetViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;

	char * valName;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 2; iCol++){

        // 注意内存释放
		valName = (char *) malloc(sizeof(char) * 128);
		memset(valName,0,sizeof(char) * 128);
		strcpy(valName,setColNames[iCol]);

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

HWND buildSetViewWindow(HWND parent){
	HINSTANCE hinst = mainModel->hInstance;//(HINSTANCE)GetWindowLong(parent,GWLP_HINSTANCE);

	RECT containerRect;
	GetClientRect (parent, &containerRect);
	
	HWND dataViewHwnd  = CreateWindowEx(0, 
		SET_VIEW_CLASS, (""), 
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

BOOL updateSetDataSet(HWND hwnd,RedisReply * reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);
    
    for (int index = 0; index < (reply->bulkSize); index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        memset(indexBuff,0,256);
        sprintf(indexBuff,"%d",(index +1));

        lvI.pszText = indexBuff; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = reply->bulks[index];
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

LRESULT CALLBACK SetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    SetViewModel * setViewModel = (SetViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            setViewModel = (SetViewModel*)malloc(sizeof(SetViewModel));
            memset(setViewModel,0,sizeof(SetViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)setViewModel);

		    HINSTANCE hinst = mainModel->hInstance;
            GetClientRect (hwnd, &rect); 

	        setViewModel->setView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,
                          rect.right - rect.left - 60,
                          rect.bottom - rect.top,
                          hwnd, NULL, hinst, NULL);

            ListView_SetExtendedListViewStyle(setViewModel->setView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
			InitSetViewColumns(setViewModel->setView);

            setViewModel->btnInsert = CreateWindowEx(0, WC_BUTTON, ("Insert"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 0, 50, 24, hwnd, (HMENU)0, 
            mainModel->hInstance, 0);

            setViewModel->btnDelete = CreateWindowEx(0, WC_BUTTON, ("Delete"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 29, 50, 24, hwnd, (HMENU)0, 
            mainModel->hInstance, 0);

            setViewModel->btnExport = CreateWindowEx(0, WC_BUTTON, ("Export"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 58, 50, 24, hwnd, (HMENU)0, 
            mainModel->hInstance, 0);

            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(setViewModel->btnInsert, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(setViewModel->btnDelete, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(setViewModel->btnExport, WM_SETFONT, (WPARAM)hfont0, FALSE);

		    break;
		}

        case WM_DT:{
            RedisReply * rp = (RedisReply *)wParam;
            updateSetDataSet(setViewModel->setView,rp);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(setViewModel->setView,0,0,rect.right-rect.left -60,rect.bottom-rect.top,TRUE);

            MoveWindow(setViewModel->btnInsert,rect.right - rect.left - 55,0, 50,24,TRUE);
            MoveWindow(setViewModel->btnDelete,rect.right - rect.left - 55,29,50,24,TRUE);
            MoveWindow(setViewModel->btnExport,rect.right - rect.left - 55,58,50,24,TRUE);
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

void init_setview(HINSTANCE hInstance){
	WNDCLASSEX setViewClass;

    setViewClass.cbSize        = sizeof(WNDCLASSEX);
    setViewClass.style         = 0;
    setViewClass.lpfnWndProc   = SetViewWndProc;
    setViewClass.cbClsExtra    = 0;
    setViewClass.cbWndExtra    = 0;
    setViewClass.hInstance     = hInstance;
    setViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    setViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    setViewClass.hbrBackground = CreateSolidBrush(RGB(240,244,244));
    setViewClass.lpszMenuName  = 0;
    setViewClass.lpszClassName = SET_VIEW_CLASS;
    setViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&setViewClass);
}
