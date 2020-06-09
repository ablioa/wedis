#include "listview.h"

HWND listView;

const char * listColNames[2]={
    "Row",
	"Value"
};

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

BOOL updateListDataSet(HWND hwnd,RedisReply * reply){
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

void buildToolBar1(HWND hwnd){
    long oldstyle;
    TBBUTTON tbtn[3] = {};
	for(int ix = 0; ix < 3 ;ix++){
		ZeroMemory(&tbtn[ix],sizeof(TBBUTTON));

		tbtn[ix].idCommand = 0;
		tbtn[ix].iBitmap = ix;
		tbtn[ix].iString = 0;
		tbtn[ix].fsState = TBSTATE_ENABLED;
		tbtn[ix].fsStyle = TBSTYLE_BUTTON | TBSTATE_WRAP;
		tbtn[ix].dwData = 0;
	}
	tbtn[0].idCommand = IDM_CONNECTION;
	tbtn[1].idCommand = IDM_TIMING;
	tbtn[2].idCommand = IDM_REMOVE;

	HINSTANCE hInst = mainModel->hInstance;
	HWND toolBarHwnd=CreateToolbarEx(
		hwnd,// parent window 
		WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT, // style
		1, // control identifier
		3, // number of imagelist
		hInst, // instance of exe file
		IDB_TOOLBAR_MAIN, // idenfier of image resource
		tbtn, // buttons
		3, // number of buttons
		14,  // width of button
		14,  // height of button
		14, // width of image
		14, // hright of image
		sizeof(TBBUTTON) // size of button structure
		);

	oldstyle = oldstyle  | TBSTYLE_WRAPABLE | CCS_VERT;
	SetWindowLong(toolBarHwnd,GWL_STYLE,oldstyle);
}

#define WEDIS_PUSH_BUTTON_STYLE BS_FLAT|WS_VISIBLE|WS_CHILD|WS_TABSTOP
#define WEDIS_COMBO_BOX_STYLE   CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE
#define WEDIS_EDIT_STYLE        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL

#define LIST_INSERT_CMD 100
#define LIST_EXPORT_CMD 101
#define LIST_DELETE_CMD 102

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
	
    switch(message){
        case WM_INITDIALOG:{
            MoveToScreenCenter(hwnd);
            MessageBox(hwnd,"sdsd","sdds",MB_OK);
            break;
        }
        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
                case LVN_COLUMNCLICK:{
                    break;
                }
                case NM_DBLCLK:{
                    DialogBox(mainModel->hInstance,MAKEINTRESOURCE (IDD_LIST_ITEM),hwnd,ListItemEditProc);
                    break;
                }
            }
            break;
        }

        // case WM_COMMAND:{
		// 	DialogBox(mainModel->hInstance,MAKEINTRESOURCE (IDD_LIST_ITEM),hwnd,ListItemEditProc);
        //     break;
        // }

	    case WM_CREATE:{
		    
            GetClientRect (hwnd, &rect);

	        listView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_VISIBLE| WS_BORDER | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,
                          rect.right - rect.left -60,
                          rect.bottom - rect.top,
                          hwnd, NULL, mainModel->hInstance, NULL);
            
            ListView_SetExtendedListViewStyle(listView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

			InitListDViewColumns(listView);

            HWND btnInsert = CreateWindowEx(0, WC_BUTTON, ("Insert"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 0, 50, 24, hwnd, LIST_INSERT_CMD, 
            mainModel->hInstance, 0);

            HWND btnDelete = CreateWindowEx(0, WC_BUTTON, ("Delete"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 29, 50, 24, hwnd, LIST_DELETE_CMD, 
            mainModel->hInstance, 0);

            HWND btnExport = CreateWindowEx(0, WC_BUTTON, ("Export"), 
            WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 58, 50, 24, hwnd, LIST_EXPORT_CMD, 
            mainModel->hInstance, 0);

            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(btnInsert, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(btnDelete, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(btnExport, WM_SETFONT, (WPARAM)hfont0, FALSE);

		    break;
		}

        case WM_COMMAND:{
            switch(LOWORD (wParam)){
                case LIST_INSERT_CMD:{
                    MessageBox(hwnd,"insert list item right now!","title",MB_OK);
                    break;
                }

                case LIST_EXPORT_CMD:{
                    MessageBox(hwnd,"export list item right now!","title",MB_OK);
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
            RedisReply * rp = (RedisReply *)wParam;
            updateListDataSet(listView,rp);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(listView,0,0,rect.right-rect.left-60,rect.bottom-rect.top,TRUE);
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
