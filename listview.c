#include "listview.h"

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

BOOL updateListDataSet(HWND hwnd,RedisReply reply){
    // char indexBuff[256] = {0};
    // LVITEM lvI;

    // lvI.pszText   = LPSTR_TEXTCALLBACK;
    // lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    // lvI.stateMask = 0;
    // lvI.iSubItem  = 0;
    // lvI.state     = 0;

    // SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);

    // RedisBulks bulks = reply->bulks;
    // for (int index = 0; index < (bulks->count); index++){
    //     lvI.iItem  = index;
    //     lvI.iImage = index;
    //     lvI.iSubItem = 0;

    //     memset(indexBuff,0,256);
    //     sprintf(indexBuff,"%d",(index +1));

    //     lvI.pszText = indexBuff; 
    //     ListView_InsertItem(hwnd, &lvI);

    //     lvI.pszText = bulks->items[index]->content;
    //     lvI.iSubItem = 1;
    //     SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    // }

    return TRUE;
}

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
	ListViewModel * listViewModel = (ListViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
	    case WM_CREATE:{
            listViewModel = (ListViewModel*)malloc(sizeof(ListViewModel));
            memset(listViewModel,0,sizeof(ListViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)listViewModel);

            GetClientRect (hwnd, &rect);

	        listViewModel->listView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_VISIBLE| WS_BORDER | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,
                          rect.right - rect.left -60,
                          rect.bottom - rect.top,
                          hwnd, NULL, mainModel->hInstance, NULL);
            
            ListView_SetExtendedListViewStyle(listViewModel->listView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

			InitListDViewColumns(listViewModel->listView);

            // listViewModel->btnInsert = CreateWindowEx(0, WC_BUTTON, ("Insert"), 
            // WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 0, 50, 24, hwnd, (HMENU)LIST_INSERT_CMD, 
            // mainModel->hInstance, 0);

            // listViewModel->btnDelete = CreateWindowEx(0, WC_BUTTON, ("Delete"), 
            // WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 29, 50, 24, hwnd, (HMENU)LIST_DELETE_CMD, 
            // mainModel->hInstance, 0);

            // listViewModel->btnExport = CreateWindowEx(0, WC_BUTTON, ("Export"), 
            // WEDIS_PUSH_BUTTON_STYLE, rect.right - rect.left - 60, 58, 50, 24, hwnd, (HMENU)LIST_EXPORT_CMD, 
            // mainModel->hInstance, 0);

            // HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            // SendMessage(listViewModel->btnInsert, WM_SETFONT, (WPARAM)hfont0, FALSE);
            // SendMessage(listViewModel->btnDelete, WM_SETFONT, (WPARAM)hfont0, FALSE);
            // SendMessage(listViewModel->btnExport, WM_SETFONT, (WPARAM)hfont0, FALSE);

		    break;
		}

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
                case LVN_COLUMNCLICK:{
                    break;
                }
                case NM_DBLCLK:{
                    DialogBox(mainModel->hInstance,MAKEINTRESOURCE (IDD_LIST_ITEM),hwnd,(DLGPROC)ListItemEditProc);
                    break;
                }
            }
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD (wParam)){
                case LIST_INSERT_CMD:{
                    MessageBox(hwnd,"insert list item right now!","title",MB_OK);
                    break;
                }

                case LIST_EXPORT_CMD:{
//                    char * filename = mGetOpenFileName(hwnd);
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
            RedisReply rp = (RedisReply)wParam;
            updateListDataSet(listViewModel->listView,rp);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(listViewModel->listView,0,0,rect.right-rect.left-60,rect.bottom-rect.top,TRUE);

            // MoveWindow(listViewModel->btnInsert,rect.right - rect.left - 55,0, 50,24,TRUE);
            // MoveWindow(listViewModel->btnDelete,rect.right - rect.left - 55,29,50,24,TRUE);
            // MoveWindow(listViewModel->btnExport,rect.right - rect.left - 55,58,50,24,TRUE);
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
