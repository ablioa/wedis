#include "listview.h"

const char * listColNames[3]={
    "Row",
    "Value",
    "Length"
};

BOOL InitListDViewColumns(HWND hWndListView) {
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int i = 0; i <= 2; i++){
        char * buff = calloc(1,255);
        strcpy(buff,listColNames[i]);

        lvc.pszText  = buff;
        lvc.cx       = 100;
        lvc.iSubItem = i;
        lvc.fmt      = LVCFMT_LEFT;
        ListView_InsertColumn(hWndListView, i, &lvc);
    }
    
    return TRUE;
}

HWND buildListToolBar(HWND parent){
    HINSTANCE hInst = App->hInstance;
    DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 3;
    TBBUTTON tbtn[3] = {
        {(TB_DELETE_BUTTON), LIST_DELETE_CMD, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), LIST_MOVE_CMD, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_EXPORT_BUTTON), LIST_EXPORT_CMD, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);
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

		char * encoded_data = encode(item->bulk->content,item->bulk->length);
        lvI.pszText = encoded_data; 
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        char buff[128] = {0};
        sprintf(buff,"%d",item->bulk->length);
        lvI.pszText = buff;
        lvI.iSubItem = 2;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
		
		free(encoded_data);
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
                    MessageBox(hwnd,"handle list row message","title",MB_OK);
                    break;
                }
            }
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD (wParam)){
                case LIST_INSERT_CMD:{
                    break;
                }

                case LIST_MOVE_CMD:{
                    MessageBox(hwnd,"moving data noe!!","title",MB_OK);
                    break;
                }

                case LIST_DELETE_CMD:{
                    char * data_key = model->dataNode->data->data_key;
                    s_db_delete_key(model->dataNode,data_key);
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
            model->dataNode = (TreeNode*) lParam;

            if(model->data->type != REPLY_MULTI){
                TreeNode * parent = model->dataNode->parent;
                s_db_get_data(parent,
                        parent->database->cursor,
                        parent->database->pattern,
                        parent->database->page_size);
                handle_redis_data(parent,NULL);
                break;
            }

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
