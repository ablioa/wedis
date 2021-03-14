#include "setview.h"

const char * setColNames[2]={
    "Row",
	"Value"
};

BOOL InitSetViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int iCol = 0; iCol < 2; iCol++){
        char * buff = (char*)calloc(1,255);
        strcpy(buff,setColNames[iCol]);

        lvc.iSubItem = iCol;
        lvc.pszText = buff;
        lvc.cx = 100;

        lvc.fmt = LVCFMT_LEFT;
        ListView_InsertColumn(hWndListView, iCol, &lvc);
    }
    
    return TRUE;
}

HWND buildSetToolBar(HWND parent){
	HINSTANCE hInst = App->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 1;
	TBBUTTON tbtn[1] = {
        {(TB_DELETE_BUTTON), 7777, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);;
}

BOOL updateSetDataSet(HWND hwnd,RedisReply reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);

    for (int index = 0; index < (reply->array_length); index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        memset(indexBuff,0,256);
        sprintf(indexBuff,"%d",(index +1));

        lvI.pszText = indexBuff; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = reply->bulks[index]->bulk->content;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

LRESULT CALLBACK SetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    SetViewModel * setViewModel = (SetViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            setViewModel = (SetViewModel*)calloc(1,sizeof(SetViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)setViewModel);
            
	        setViewModel->setView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, App->hInstance, NULL);
            ListView_SetExtendedListViewStyle(setViewModel->setView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
			InitSetViewColumns(setViewModel->setView);

            setViewModel->toolBar = buildSetToolBar(hwnd);
		    break;
		}

        case WM_DT:{
            RedisReply rp = (RedisReply)wParam;
            updateSetDataSet(setViewModel->setView,rp);
            break;
        }

		case WM_COMMAND:{
			switch(LOWORD(wParam)){
			    case 7777:{
					MessageBox(hwnd,"remove set item","title",MB_OK);
					break;
			    }
			}

			break;
		}
		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
            MoveWindow(setViewModel->toolBar,0,0,rect.right-rect.left,28,TRUE);
			MoveWindow(setViewModel->setView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
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
    setViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    setViewClass.lpszMenuName  = 0;
    setViewClass.lpszClassName = SET_VIEW_CLASS;
    setViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&setViewClass);
}
