#include "setview.h"

const char * setColNames[3]={
    "Row",
    "Value",
    "Length"
};

BOOL InitSetViewColumns(HWND hWndListView) { 
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int iCol = 0; iCol < 3; iCol++){
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


		char * encoded_data = encode(reply->bulks[index]->bulk->content,
				reply->bulks[index]->bulk->length);

        lvI.pszText = encoded_data;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        char buff[128] = {};
        sprintf(buff,"%d",reply->bulks[index]->bulk->length);
        lvI.pszText = buff;
        lvI.iSubItem = 2;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

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
            InitSetViewColumns(model->setView);

            model->toolBar = build_toolbar(hwnd);
            break;
        }

        case WM_DT:{
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
                    s_db_delete_key(model->dataNode,data_key);
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
