#include "zsetview.h"

const ColumnAttribute zset_column[2] ={
    {200,IDS_LV_COLUMN_ZSET_ELEMENT},
    {200, IDS_LV_COLUMN_ZSET_SCORE}
};

HWND buildZsetToolBar(HWND parent){
    int buttonCount = 3;
    TBBUTTON tbtn[3] = {
        {(TB_REFRESH_BUTTON), TB_CMD_REFRESH_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), TB_CMD_MOVE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_DELETE_BUTTON), TB_CMD_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);
}

LRESULT CALLBACK ZsetViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    ZsetViewModel * model = (ZsetViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
    switch(message){
        case WM_CREATE:{
            model = (ZsetViewModel*)calloc(1,sizeof(ZsetViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            HINSTANCE hinst = App->hInstance;
            GetClientRect (hwnd, &rect); 
            
            model->zsetView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, hinst, NULL);

            model->toolBar = buildZsetToolBar(hwnd);
            ListView_SetExtendedListViewStyle(model->zsetView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            
            LVCOLUMN lvc;
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            for (int i = 0; i < 2; i++){
                char * buff = (char *)calloc(1,255);
                LoadString(App->hInstance,zset_column[i].columnId,buff,255);

                lvc.pszText  = buff;
                lvc.cx       = zset_column[i].width;
                lvc.iSubItem = i;
                lvc.fmt      = LVCFMT_LEFT;
                ListView_InsertColumn(model->zsetView, i, &lvc);
                free(buff);
            }

            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case TB_CMD_DELETE_DATA:{
                    char * data_key = model->dataNode->data->data_key;
                    delete_data_node(model->dataNode,data_key);
                    break;
                }
            }
        }

        case WM_SIZE:{
            GetClientRect(hwnd,&rect);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
            MoveWindow(model->zsetView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);

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

            UpdateZsetData(model->zsetView,model->data);
            break;
        }
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

BOOL UpdateZsetData(HWND hwnd,RedisReply reply){
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);

    for (int index = 0; index < (reply->array_length / 2); index++){
        lvI.iItem  = index;
        lvI.iImage = index;

        lvI.iSubItem = 0;

        lvI.pszText = reply->bulks[index * 2]->bulk->content; 
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = reply->bulks[index *2+1]->bulk->content;;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

void init_zsetview(HINSTANCE hInstance){
    WNDCLASSEX zsetViewClass;

    zsetViewClass.cbSize        = sizeof(WNDCLASSEX);
    zsetViewClass.style         = 0;
    zsetViewClass.lpfnWndProc   = ZsetViewWndProc;
    zsetViewClass.cbClsExtra    = 0;
    zsetViewClass.cbWndExtra    = 0;
    zsetViewClass.hInstance     = hInstance;
    zsetViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    zsetViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    zsetViewClass.hbrBackground = resource->brush; 
    zsetViewClass.lpszMenuName  = 0;
    zsetViewClass.lpszClassName = ZSET_VIEW_CLASS;
    zsetViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&zsetViewClass);
}
