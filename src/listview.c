#include "listview.h"

const ColumnAttribute list_column[1] ={
    {450,IDS_LV_COLUMN_LIST_ITEM}
};

HWND buildListToolBar(HWND parent){
    int buttonCount = 4;
    TBBUTTON tbtn[4] = {
        {(TB_REFRESH_BUTTON), TB_CMD_REFRESH_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), TB_CMD_MOVE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_DELETE_BUTTON), TB_CMD_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_EXPORT_BUTTON), TB_CMD_EXPORT_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);
}

BOOL updateListDataSet(HWND hwnd,RedisReply reply){
    char * encoded_data;
    int xlen;
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

       encoded_data = encode(item->bulk->content,item->bulk->length,&xlen);
       lvI.pszText = encoded_data;
       lvI.cchTextMax = xlen;
       ListView_InsertItem(hwnd, &lvI);

	   free(encoded_data);
    }

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
                          WS_CHILD | WS_VISIBLE| WS_BORDER | LVS_REPORT | LVS_EDITLABELS | LVS_SHAREIMAGELISTS,
                          0, 0, 0, 0,
                          hwnd, NULL, App->hInstance, NULL);

            model->toolBar = buildListToolBar(hwnd);
            ListView_SetExtendedListViewStyle(model->listView,LVS_EX_INFOTIP|LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

            // init list columns
            LVCOLUMN lvc;
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            for (int i = 0; i < 1; i++){
                char * buff = (char *)calloc(1,255);
                LoadString(App->hInstance,list_column[i].columnId,buff,255);

                lvc.pszText  = buff;
                lvc.cx       = list_column[i].width;
                lvc.iSubItem = i;
                lvc.fmt      = LVCFMT_LEFT;
                ListView_InsertColumn(model->listView, i, &lvc);
                free(buff);
            }

            break;
        }

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
            switch (msg->code) {
                case NM_DBLCLK:{
                    // TODO double click on list item
                    break;
                }

                case TTN_GETDISPINFO:{
                    LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
                    lpttt->hinst = App->hInstance;
                    UINT_PTR idButton = lpttt->hdr.idFrom;
                    switch(idButton){
                        case TB_CMD_REFRESH_DATA:{
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_REFRESH_LIST_DATA);
                            break;
                        }
                        case TB_CMD_MOVE_DATA:{
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_MOVE_LIST_DATA);
                            break;
                        };
                        case TB_CMD_DELETE_DATA:{
                            lpttt->lpszText =MAKEINTRESOURCE(IDS_TB_DELETE_LIST_DATA);
                            break;
                        }
                        case TB_CMD_EXPORT_DATA:{
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_EXPORT_LIST_DATA);
                            break;
                        }
                    }
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

                case TB_CMD_MOVE_DATA:{
                    break;
                }

                case TB_CMD_DELETE_DATA:{
                    char * data_key = model->dataNode->data->data_key;
                    delete_data_node(model->dataNode,data_key);
                    break;
                }

                case TB_CMD_REFRESH_DATA:{
                    TreeNode * node = model->dataNode;
                    s_handle_data(node,node->data->data_type);
                    break;
                }

                case TB_CMD_EXPORT_DATA:{
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
            free_redis_reply(model->data);

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
    listViewClass.hbrBackground = resource->brush;
    listViewClass.lpszMenuName  = 0;
    listViewClass.lpszClassName = LIST_VIEW_CLASS;
    listViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));

    RegisterClassEx(&listViewClass);
}
