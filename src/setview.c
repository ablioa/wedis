#include "setview.h"

const ColumnAttribute set_column[1] ={
    {200,IDS_LV_COLUMN_SET_ELEMENT}
};

static HWND build_toolbar(HWND parent){
    int buttonCount = 3;
    TBBUTTON tbtn[3] = {
        {(TB_REFRESH_BUTTON), TB_CMD_REFRESH_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_DELETE_BUTTON), TB_CMD_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), TB_CMD_MOVE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };
    return buildGeneralToolBar(parent,tbtn,buttonCount);;
}

BOOL updateSetDataSet(HWND hwnd,RedisReply reply){
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
        
        int xlen;
        char * encoded_data = encode(reply->bulks[index]->bulk->content,reply->bulks[index]->bulk->length,&xlen);
        lvI.pszText = encoded_data;
        lvI.iSubItem = 0;
        ListView_InsertItem(hwnd, &lvI);

        free(encoded_data);
    }

    return TRUE;
}

LRESULT CALLBACK SetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    SetViewModel * model = (SetViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
        case WM_CREATE:{
            model = (SetViewModel*)calloc(1,sizeof(SetViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            model->setView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, App->hInstance, NULL);
            ListView_SetExtendedListViewStyle(model->setView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

            LVCOLUMN lvc;
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            for (int i = 0; i < 1; i++){
                char * buff = (char *)calloc(1,255);
                LoadString(App->hInstance,set_column[i].columnId,buff,255);

                lvc.pszText  = buff;
                lvc.cx       = set_column[i].width;
                lvc.iSubItem = i;
                lvc.fmt      = LVCFMT_LEFT;
                ListView_InsertColumn(model->setView, i, &lvc);
                free(buff);
            }

            model->toolBar = build_toolbar(hwnd);
            break;
        }

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
            switch (msg->code) {
                case TTN_GETDISPINFO:{
                    LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
                    lpttt->hinst = App->hInstance;
                    UINT_PTR idButton = lpttt->hdr.idFrom;
                    switch(idButton){
                        case TB_CMD_REFRESH_DATA:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_REFRESH_SET_DATA);break;}
                        case TB_CMD_MOVE_DATA:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_MOVE_SET_DATA);break;};
                        case TB_CMD_DELETE_DATA:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_DELETE_SET_DATA);;break;}
                    }
                }
                break;
            }
            break;
        }
        case WM_DT:{
            free_redis_reply(model->data);
            model->data = (RedisReply)wParam;
            model->dataNode = (TreeNode*)lParam;

            if(model->data->type != REPLY_MULTI){
                TreeNode * parent = model->dataNode->parent;
                s_db_get_data(parent,
                        parent->database->cursor,
                        parent->database->pattern,
                        parent->database->page_size);
                handle_redis_data(parent,NULL);
                break;
            }

            updateSetDataSet(model->setView,model->data);
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case TB_CMD_MOVE_DATA:{
                    MessageBox(hwnd,"move hash data right now.","title",MB_OK);
                    break;
                }

                case TB_CMD_REFRESH_DATA:{
                    MessageBox(hwnd,"move refresh data right now.","title",MB_OK);
                    break;
                }

                case TB_CMD_DELETE_DATA:{
                    char * data_key = model->dataNode->data->data_key;
                    delete_data_node(model->dataNode,data_key);
                    break;
                }
            }

            break;
        }
        case WM_SIZE:{
            GetClientRect(hwnd,&rect);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
            MoveWindow(model->setView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
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
    setViewClass.hbrBackground = resource->brush; 
    setViewClass.lpszMenuName  = 0;
    setViewClass.lpszClassName = SET_VIEW_CLASS;
    setViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&setViewClass);
}
