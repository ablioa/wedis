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

static HWND build_toolbar(HWND parent){
    int buttonCount = 3;
    TBBUTTON tbtn[3] = {
        {(TB_REFRESH_BUTTON), TB_CMD_REFRESH_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_DELETE_BUTTON), TB_CMD_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), TB_CMD_MOVE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
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

        RedisBulk bulk = reply->bulks[index*2]->bulk;

        int xlen;
        char * encoded_text = encode(bulk->content,bulk->length,&xlen);
        lvI.pszText = encoded_text;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        bulk = reply->bulks[index*2+1]->bulk;
        int len;
        encoded_text = encode(bulk->content,bulk->length,&len);
        lvI.pszText = encoded_text;
        lvI.iSubItem = 2;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
    }

    return TRUE;
}

LRESULT CALLBACK HashViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    HashViewModel * model = (HashViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
        case WM_CREATE:{
            model = (HashViewModel*)calloc(1,sizeof(HashViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            model->hashView = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, App->hInstance, NULL);

            ListView_SetExtendedListViewStyle(model->hashView,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            InitHashViewColumns(model->hashView);

            model->toolbar = build_toolbar(hwnd);
            break;
        }

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
            switch(msg->code){
                case TTN_GETDISPINFO:{
                    LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
                    lpttt->hinst = App->hInstance;
                    UINT_PTR idButton = lpttt->hdr.idFrom;
                    switch(idButton){
                        case TB_CMD_REFRESH_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_HSH_REFRESH_DATA);
                        break;
                        case TB_CMD_MOVE_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_HSH_MOVE_DATA);
                        break;
                        case TB_CMD_DELETE_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_HSH_DELETE_DATA);
                        break;
                    }
                }
                break;
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
            updateHashDataSet(model->hashView,model->data);
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
            MoveWindow(model->toolbar,0,0,rect.right-rect.left,28,TRUE);
            MoveWindow(model->hashView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
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
    hashViewClass.hbrBackground = resource->brush;
    hashViewClass.lpszMenuName  = 0;
    hashViewClass.lpszClassName = HASH_VIEW_CLASS;
    hashViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    
    RegisterClassEx(&hashViewClass);
}

