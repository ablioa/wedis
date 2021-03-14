#include "hashview.h"

const char * colNames[3]={
    "Row",
	"Key",
	"Value"
};

BOOL InitHashViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 3; iCol++){
        char * buff = (char *)calloc(1,255);
        strcpy(buff,colNames[iCol]);
        lvc.iSubItem = iCol;
        lvc.pszText = buff;
        lvc.cx = 100;
        lvc.fmt = LVCFMT_LEFT;

        ListView_InsertColumn(hWndListView, iCol, &lvc);
    }
    
    return TRUE;
}

HWND buildHashToolBar(HWND parent){
    int buttonCount = 1;
	TBBUTTON tbtn[1] = {
        {(TB_DELETE_BUTTON), 7777, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);
}

BOOL updateHashDataSet(HWND hwnd,RedisReply reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);

    int total = reply->array_length;

    for(int index =0; index < (total/2); index ++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        memset(indexBuff,0,256);
        sprintf(indexBuff,"%d",(index +1));

        lvI.pszText = indexBuff; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = reply->bulks[index*2]->bulk->content;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        lvI.pszText = reply->bulks[index*2+1]->bulk->content;
        lvI.iSubItem = 2;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}


LRESULT CALLBACK HashViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    HashViewModel * hashViewModel = (HashViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            hashViewModel = (HashViewModel*)calloc(1,sizeof(HashViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)hashViewModel);
	        
            hashViewModel->hashView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, App->hInstance, NULL);
            
            ListView_SetExtendedListViewStyle(hashViewModel->hashView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
			InitHashViewColumns(hashViewModel->hashView);

            hashViewModel->toolBar = buildHashToolBar(hwnd);
		    break;
		}

        case WM_DT:{
            RedisReply rp = (RedisReply)wParam;
            updateHashDataSet(hashViewModel->hashView,rp);
            break;
        }

		case WM_COMMAND:{
			switch(LOWORD(wParam)){
			    case 7777:{
					MessageBox(hwnd,"remove hash item","title",MB_OK);
					break;
			    }
			}

			break;
		}
		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
            MoveWindow(hashViewModel->toolBar,0,0,rect.right-rect.left,28,TRUE);
			MoveWindow(hashViewModel->hashView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
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

