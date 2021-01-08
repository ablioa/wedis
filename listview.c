#include "listview.h"

const char * listColNames[2]={
    "Row",
	"Value"
};

HWND buildListToolBar(HWND parent){
	HINSTANCE hInst = App->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 4;
	TBBUTTON tbtn[4] = {
        {(0), LIST_EXPORT_CMD, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(1), IDM_PREFERENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(2), IDM_PREFERENCE+1, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_PREFERENCE+2, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_LISTTB));
    HIMAGELIST hIcons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, buttonCount);
	ImageList_AddMasked(hIcons, hBmp, RGB(255,255,255));

    HWND tb = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle, 16, 16, 16, 16, parent, (HMENU) 0, hInst, NULL);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) hIcons);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_ADDBUTTONS,       (WPARAM)buttonCount,       (LPARAM)&tbtn);
    SendMessage(tb, TB_AUTOSIZE, 0, 0);

    ShowWindow(tb,  TRUE);

    return tb;
}

BOOL InitListDViewColumns(HWND hWndListView) {
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    
    for (int i = 0; i < 2; i++){
        lvc.pszText  = listColNames[i];
        lvc.cx       = 100;
        lvc.iSubItem = i;
        lvc.fmt      = LVCFMT_LEFT;
        ListView_InsertColumn(hWndListView, i, &lvc);
    }
    
    return TRUE;
}

BOOL updateListDataSet(HWND hwnd,RedisReply reply){
    char indexBuff[256] = {0};
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    ListView_DeleteAllItems(hwnd);

    for(int ix = 0; ix < reply->array_length; ix ++){
        RedisReply item = reply->bulks[ix];
        
        lvI.iItem  = ix;
        lvI.iImage = ix;
        lvI.iSubItem = 0;

        memset(indexBuff,0,256);
        sprintf(indexBuff,"%d",(ix +1));

        lvI.pszText = indexBuff; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = item->bulk->content;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    // TODO 这里还不能释放，需要选择合适的时机
    // free_redis_reply(reply);

    return TRUE;
}

LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
	ListViewModel * model = (ListViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
	    case WM_CREATE:{
            model = (ListViewModel*)calloc(1,sizeof(ListViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

	        model->listView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_VISIBLE| WS_BORDER | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0, 0, 0,
                          hwnd, NULL, App->hInstance, NULL);

            model->toolBar = buildListToolBar(hwnd);
            ListView_SetExtendedListViewStyle(model->listView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
			InitListDViewColumns(model->listView);
		    break;
		}

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
                case LVN_COLUMNCLICK:{
                    break;
                }
                case NM_DBLCLK:{
                    DialogBox(App->hInstance,MAKEINTRESOURCE (IDD_LIST_ITEM),hwnd,(DLGPROC)ListItemEditProc);
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

                case LIST_DELETE_CMD:{
                    MessageBox(hwnd,"delete list item right now!","title",MB_OK);
                    break;
                }

                case LIST_EXPORT_CMD:{
                    char * filename = mGetSaveFileName(hwnd);
                    if(filename == NULL){
                        break;
                    }

                    FILE * stream = fopen(filename,"w");
                    int len = model->data->array_length;
                    for(int ix = 0; ix < len; ix ++){
                        RedisReply item = model->data->bulks[ix];
                        fprintf(stream,"%s\n",item->bulk->content);
                    }
                    fclose(stream);

                    break;
                }
            }
            break;
        }

        case WM_DT:{
            model->data = (RedisReply)wParam;
            updateListDataSet(model->listView,model->data);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
			MoveWindow(model->listView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
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
