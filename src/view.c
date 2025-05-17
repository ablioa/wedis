#include "view.h"

#include <windowsx.h>
#define VIEW_SIZE 7

widget views[VIEW_SIZE]={
    {NULL,{0,0,0,0},HASH_VIEW_CLASS,    "",(!WS_VISIBLE) | WS_CHILD,(HMENU)6,NULL,REDIS_HASH},
    {NULL,{0,0,0,0},STRING_VIEW_CLASS,  "",(!WS_VISIBLE) | WS_CHILD,(HMENU)7,NULL,REDIS_STRING},
    {NULL,{0,0,0,0},LIST_VIEW_CLASS,    "",(!WS_VISIBLE) | WS_CHILD,(HMENU)8,NULL,REDIS_LIST},
    {NULL,{0,0,0,0},SET_VIEW_CLASS,     "",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_SET},
    {NULL,{0,0,0,0},ZSET_VIEW_CLASS,    "",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_ZSET},
    {NULL,{0,0,0,0},SYSTEM_VIEW_CLASS,  "",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_SYSTEM},
    {NULL,{0,0,0,0},DATABASE_VIEW_CLASS,"",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_DATABASE}
};

LRESULT CALLBACK dataViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    DataView * dataView = App->dataView;

    switch(msg){
        case WM_CREATE:{
            for(int i = 0; i < VIEW_SIZE; i ++){
                widget * wgt = &views[i];
                create_widget(hwnd,App->hInstance,wgt);
            }

            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)dataView);
            break;
        }

        case WM_DTT:{
            SendMessage(dataView->visibleHwnd,WM_DTT,wParam,lParam);
            break;
        }

        case WM_DT:{
            RedisReply data     = (RedisReply) wParam;
            TreeNode * datanode = (TreeNode *) lParam;

            int dataType = REDIS_SYSTEM;
            if(datanode->level == NODE_LEVEL_HOST){
                dataType = REDIS_SYSTEM;
            }else if(datanode->level == NODE_LEVEL_DATABASE){
                dataType = REDIS_DATABASE;
            }else{
                dataType = datanode->data->data_type;
            }

            switchView(hwnd,dataType,datanode,data);
            break;
        }

        case WM_SIZE:{
            RECT rect;
            GetClientRect(hwnd,&rect);
            for(int i = 0; i < VIEW_SIZE; i ++){
                widget * wgt = &views[i];
                MoveWindow(wgt->hwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            }
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void switchView(HWND hwnd,int type,TreeNode * treeNode,RedisReply data){
    for (int i = 0; i < 7; i++){
        int visible = SW_HIDE;
        if(views[i].type == type){
            visible = SW_SHOW;
            SendMessage(views[i].hwnd,WM_DT,(WPARAM)data,(LPARAM)treeNode);
        }

        ShowWindow(views[i].hwnd,visible);
    }

}

#define GAP_WIDTH 5

widget ttl_row[5]={
    {NULL,{0,  30,40,24},WC_STATIC,"TTL:",WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT,(HMENU)1},
    {NULL,{45, 30,-1,24},WC_EDIT,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,(HMENU)2},
    {NULL,{170,30,40,24},WC_BUTTON,"ms",WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000003,(HMENU)3},
    {NULL,{255,30,60,24},WC_BUTTON,"Update",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)4},
    {NULL,{320,30,60,24},WC_BUTTON,"Persist",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)5}
};

widget keys_row[4]={
    {NULL,{0,  5,40,24},WC_STATIC,"KEY:",WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT,(HMENU)6},
    {NULL,{45, 5,-1,24},WC_EDIT,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,(HMENU)7},
    {NULL,{255,5,60,24},WC_BUTTON,"Import",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)8},
    {NULL,{320,5,60,24},WC_BUTTON,"Update",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)9}
};

static HWND build_string_toolbar(HWND parent){
    int buttonCount = 7;
    TBBUTTON tbtn[7] = {
        {(TB_REFRESH_BUTTON), TB_CMD_REFRESH_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), TB_CMD_MOVE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_DELETE_BUTTON), TB_CMD_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(0), 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
        {(TB_EXPORT_BUTTON), TB_CMD_EXPORT_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_FORMAT_BUTTON), TB_CMD_FORMAT_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_WIDEN_BUTTON),  TB_CMD_WIDEN_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);;
}

int get_next_hex_width(StringViewModel * model){
    model->hex_width += HEX_WIDTH_STEP;

    if(model->hex_width > MAX_HEX_WIDTH){
        model->hex_width = MIN_HEX_WIDTH;
    }

    return model->hex_width;
}

LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    StringViewModel * model = (StringViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
        case WM_CREATE:{
            model = (StringViewModel*)malloc(sizeof(StringViewModel));
            memset(model,0,sizeof(StringViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            model->stringView = CreateWindowEx(0, WC_EDIT, (""), 
                    WS_VISIBLE | ES_AUTOVSCROLL | WS_BORDER | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL |ES_MULTILINE, 
                    0, 0, 0, 0, hwnd, (HMENU)IDC_STRING_VIEW_TEXT, App->hInstance, 0);
            model->propertyView   = CreateWindowEx(0, STRING_VIEW_PROP_CLASS,NULL, 
                    (WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                    0,0,0,0,hwnd, (HMENU)0, App->hInstance, 0);
            model->toolBar = build_string_toolbar(hwnd);
            model->mode = TEXT;
            model->hex_width=16;

            SendMessage(model->stringView, WM_SETFONT, (WPARAM)(resource->fixedWidthFont), FALSE);
            Edit_SetReadOnly(model->stringView,TRUE);
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
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_STR_REFRESH_DATA);
                        break;
                        case TB_CMD_MOVE_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_STR_MOVE_DATA);
                        break;
                        case TB_CMD_DELETE_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_STR_DELETE_DATA);
                        break;
                        case TB_CMD_EXPORT_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_STR_EXPORT_DATA);
                        break;
                        case TB_CMD_FORMAT_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_STR_FORMAT_DATA);
                        break;
                        case TB_CMD_WIDEN_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_STR_WIDEN_DATA);
                        break;
                    }
                }
                break;
            }

            break;
        }

        case WM_COMMAND:{
            switch(LOWORD (wParam)){
                /** move data into another database */
                case WM_MOVEDATA_CMD:{
                    // TODO add move data operation
                    break;
                }

                /** export data */
                case TB_CMD_EXPORT_DATA:{
                    LPTSTR fileName = mGetSaveFileName(hwnd);
                    if(fileName == NULL){
                        break;
                    }

                    FILE * stream = fopen(fileName,"wb");
                    fwrite(model->data->bulk->content,1,model->data->bulk->length,stream);
                    fclose(stream);
                    break;
                }

                case TB_CMD_WIDEN_DATA:{
                    if(model->mode == BINARY){
                        get_next_hex_width(model);
                        char * content = model->data->bulk->content;
                        int len = model->data->bulk->length;
                        content = dump_text(model->data->bulk->content,len,model->hex_width);
                        SetWindowText(model->stringView,content);
                    }
                    break;
                }

                /** display data in hex dump format*/
                case TB_CMD_FORMAT_DATA:{
                    char * content = model->data->bulk->content;
                    if(model->mode == TEXT){
                        model->mode = BINARY;
                        int len = model->data->bulk->length;
                        content = dump_text(model->data->bulk->content,len,model->hex_width);
                    }else{
                        model->mode = TEXT;
                    }

                    SetWindowText(model->stringView,content);
                    break;
                }

                case TB_CMD_REFRESH_DATA:{
                    TreeNode * node = model->dataNode;
                    s_handle_data(node,node->data->data_type);
                    break;
                }

                /** delete data item */
                case TB_CMD_DELETE_DATA:{
                    char * data_key = model->dataNode->data->data_key;
                    delete_data_node(model->dataNode,data_key);
                    break;
                }

                /** trigger move data menu */
                case TB_CMD_MOVE_DATA:{
                    POINT pt;
                    GetCursorPos(&pt);
                    TrackPopupMenu(model->dataNode->parent->database->db_menu,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);
                    break;
                }
            }
            break;
        }

        case WM_DT:{
            free_redis_reply(model->data);

            model->data = (RedisReply)wParam;
            model->dataNode = (TreeNode*)lParam;

            if(model->data->type != REPLY_BULK){
                TreeNode * parent = model->dataNode->parent;
                s_db_get_data(parent,
                        parent->database->cursor,
                        parent->database->pattern,
                        parent->database->page_size);
                handle_redis_data(parent,NULL);
                break;
            }

            SendMessage(model->propertyView,WM_DT,(WPARAM)model->data,(LPARAM)model->dataNode);
            char * text = model->data->bulk->content;
            int length  = model->data->bulk->length;
            if(is_binary_data(text,length)){
                model->mode = BINARY;
            }

            if(model->mode == BINARY){
                text = dump_text(text,length,model->hex_width);
                SetWindowText(model->stringView,text);
                free(text);
            }else{
                SetWindowText(model->stringView,text);
            }

            break;
        }

        case WM_SIZE:{
            GetClientRect(hwnd,&rect);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
            MoveWindow(model->stringView,0,28,rect.right-rect.left,rect.bottom-rect.top-28-100,TRUE);
            MoveWindow(model->propertyView,0,rect.bottom-rect.top-100,rect.right-rect.left,100,TRUE);
            break;
        }
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

/** TODO formula design */
LRESULT CALLBACK stringViewPropWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    PropertyModel * model = (PropertyModel*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
    switch(message){
        case WM_CREATE:{
            HINSTANCE hinst = App->hInstance;
            model = (PropertyModel*) calloc(1,sizeof(PropertyModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            for(int ix = 0; ix < 5; ix ++){
                create_widget(hwnd,hinst,&ttl_row[ix]);
            }

            for(int ix = 0; ix < 4; ix ++){
                create_widget(hwnd,hinst,&keys_row[ix]);
            }

            EnumChildWindows(hwnd,enumChildProc,0);
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case 8:{
                    LPTSTR file_name = mGetOpenFileName(hwnd);
                    if(file_name == NULL){
                        break;
                    }
                    break;
                }

                case 9:{
                    char * buff = (char*) calloc(1,MAX_PATH);
                    GetDlgItemText(hwnd,7,buff,MAX_PATH);

                    char * old_key = model->dataNode->data->data_key;
                    redis_rename_key(model->dataNode,old_key,buff);
                    break;
                }
            }

            break;
        }

        case WM_DT:{
            model->data = (RedisReply)wParam;
            model->dataNode = (TreeNode*)lParam;
            SetDlgItemText(hwnd,7,model->dataNode->data->data_key);
        }

        case WM_SIZE:{
            arrange_widgets(hwnd,keys_row,4);
            arrange_widgets(hwnd,ttl_row,5);
            break;
        }
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

void init_stringview(HINSTANCE hInstance){
    WNDCLASSEX stringViewClass;

    stringViewClass.cbSize        = sizeof(WNDCLASSEX);
    stringViewClass.style         = 0;
    stringViewClass.cbClsExtra    = 0;
    stringViewClass.cbWndExtra    = 0;
    stringViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    stringViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    stringViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    stringViewClass.hbrBackground = resource->brush; 
    stringViewClass.lpszMenuName  = 0;

    stringViewClass.lpszClassName = STRING_VIEW_CLASS;
    stringViewClass.lpfnWndProc   = StringViewWndProc;
    stringViewClass.hInstance     = hInstance;

    RegisterClassEx(&stringViewClass);

    stringViewClass.lpszClassName = STRING_VIEW_PROP_CLASS;
    stringViewClass.lpfnWndProc   = stringViewPropWndProc;
    stringViewClass.hInstance     = hInstance;

    RegisterClassEx(&stringViewClass);
}

char * get_output_buffer(int len,int width){
    int bsize = (10+width+4+width*3+2) * (len / width +1) +100;
    char * buff = ( char *) malloc(sizeof( char) * bsize);
    memset(buff,0,sizeof( char) * bsize);
    return buff;
}

char * dump_text(char * text,int len,int width){
    char * line   = (char*)calloc(1,width+1);
    char * buff   = get_output_buffer(len,width);
    char * output = buff;

    int offset =0;
    int ix = 0;
    for(ix = 0; ix < len; ix ++){
        if(ix % width ==0){
            sprintf(output,"%08X: ",(ix/width * width));
            output += 10;
        }

        sprintf(output,"%02X ",(unsigned char)(text[ix]));
        output +=3;
        line[offset++]= isprint(text[ix])?text[ix]:'.';

        if(ix % width == (width-1)){
            sprintf(output,"    %s\r\n",line);
            offset=0;
            memset(line,0,(width+1));
            output += (width+4+2); 
        }
    }

    if(ix % width != 0x00){
        int left  = len % width;
        int vleft = width - left;

        for(int iz = 0; iz < vleft; iz ++){
           sprintf(output,"   ");
           output += 3;
        }

        sprintf(output,"    %s\r\n",line);
        offset=0;
        memset(line,0,(width+1));
        output += (width+4+2);
    }

    free(line);

    return buff;
}

/**
 * build widget window
 */
void create_widget(HWND parent,HINSTANCE hinst,widget * wgt){
    wgt->hwnd = CreateWindowEx(0, 
            wgt->window_class, wgt->name, wgt->style, 
            wgt->position.left,
            wgt->position.top,
            wgt->position.width,
            wgt->position.height,
            parent, 
            (HMENU)(wgt->id), 
            hinst, 
            0);

    if(wgt->editorProc != NULL){
        SetWindowLongPtr(wgt->hwnd,GWLP_WNDPROC,(LONG_PTR)wgt->editorProc);
    }

    if(wgt->hwnd == NULL){
        DumpMessage(0x0005);
    }
}

void arrange_widgets(HWND hwnd,widget wgts[],int length){
    RECT prect;
    GetClientRect(hwnd,&prect);
    int totalwidth = 0;

    for(int ix = 0; ix < length; ix ++){
        widget wgt = wgts[ix];
        if(wgt.position.width != -1){
            totalwidth += wgt.position.width;
        }
    }

    totalwidth += (length+1) * GAP_WIDTH;
    int twidth = prect.right - totalwidth;
   
    int next_left = 0;
    for(int ix = 0; ix < length; ix ++){
        widget wgt = wgts[ix];
        dimension pos = wgt.position;

        next_left += GAP_WIDTH;
        if(pos.width == -1){
            pos.width = twidth;
        }

        MoveWindow(wgt.hwnd,next_left,pos.top,
                pos.width,pos.height,TRUE);
        next_left += pos.width;
    }
}

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

const ColumnAttribute hash_column[2] ={
    {200,IDS_LV_COLUMN_HASH_KEY},
    {200, IDS_LV_COLUMN_HASH_VALUE}
};

static HWND build_hash_toolbar(HWND parent){
    int buttonCount = 3;
    TBBUTTON tbtn[3] = {
        {(TB_REFRESH_BUTTON), TB_CMD_REFRESH_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_DELETE_BUTTON), TB_CMD_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), TB_CMD_MOVE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);
}

BOOL updateHashDataSet(HWND hwnd,RedisReply reply){
    int xlen;
    LVITEM lvI;
    char * encode_content = NULL;

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

        RedisBulk bulk = reply->bulks[index*2]->bulk;
        encode_content = encode(bulk->content,bulk->length,&xlen);
        lvI.pszText = encode_content;
        lvI.cchTextMax = xlen;
        lvI.iSubItem = 0;
        ListView_InsertItem(hwnd, &lvI);

        free(encode_content);
 
        bulk = reply->bulks[index*2+1]->bulk;
        encode_content = encode(bulk->content,bulk->length,&xlen);
        lvI.pszText = encode_content;
        lvI.cchTextMax = xlen;
        lvI.iSubItem = 1;
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        free(encode_content);
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
            //InitHashViewColumns(model->hashView);

            // init list columns
            LVCOLUMN lvc;
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            for (int i = 0; i < 2; i++){
                char * buff = (char *)calloc(1,255);
                LoadString(App->hInstance,hash_column[i].columnId,buff,255);

                lvc.pszText  = buff;
                lvc.cx       = hash_column[i].width;
                lvc.iSubItem = i;
                lvc.fmt      = LVCFMT_LEFT;
                ListView_InsertColumn(model->hashView, i, &lvc);
                free(buff);
            }

            model->toolbar = build_hash_toolbar(hwnd);
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

            updateHashDataSet(model->hashView,model->data);
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case TB_CMD_MOVE_DATA:{
                    break;
                }

                case TB_CMD_REFRESH_DATA:{
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
                    break;
                }

                case TB_CMD_REFRESH_DATA:{
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

widget query_row[5]={
    {NULL,{5,  28,80,24},WC_EDIT,"0",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER|ES_AUTOHSCROLL,(HMENU)DB_CTRL_CURSOR},
    {NULL,{90, 28,-1,24},WC_EDIT,"*",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER|ES_AUTOHSCROLL,(HMENU)DB_CTRL_PATTERN},
    {NULL,{175,28,80,24},WC_EDIT,"10",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER|ES_AUTOHSCROLL,(HMENU)DB_CTRL_COUNT},
    {NULL,{260,28,60,24},WC_BUTTON,"Scan",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)DB_CTRL_SEARCH},
    {NULL,{325,28,60,24},WC_BUTTON,"Next",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)DB_CTRL_NEXT_SEARCH}
};

void init_database_view(HINSTANCE hInstance){
    WNDCLASSEX hashViewClass;

    hashViewClass.cbSize        = sizeof(WNDCLASSEX);
    hashViewClass.style         = 0;
    hashViewClass.lpfnWndProc   = DatabaseViewWndProc;
    hashViewClass.cbClsExtra    = 0;
    hashViewClass.cbWndExtra    = 0;
    hashViewClass.hInstance     = hInstance;
    hashViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    hashViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    hashViewClass.hbrBackground = resource->brush;
    hashViewClass.lpszMenuName  = 0;
    hashViewClass.lpszClassName = DATABASE_VIEW_CLASS;
    hashViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));

    RegisterClassEx(&hashViewClass);
}

static HWND buildDatabaseToolBar(HWND parent){
    int buttonCount = 7;
    TBBUTTON tbtn[7] = {
        {(TB_DELETE_BUTTON), TB_CMD_DELETE_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_ADD_BUTTON), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},

        {(TREE_DATA_NODE_TEXT), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TREE_DATA_NODE_LIST), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TREE_DATA_NODE_HASH), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TREE_DATA_NODE_SET), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TREE_DATA_NODE_ZSET), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
    };
    return buildGeneralToolBar(parent,tbtn,buttonCount);
}

BOOL CALLBACK enumChildProc(HWND hwnd,LPARAM lParam){
	// HFONT systemFont = GetStockObject(SYSTEM_FONT);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);

    char buff[255] = {0};
    GetClassName(hwnd,buff,255);
    if(strcmp(buff,"Button") == 0){
        SetWindowLongPtr(hwnd,GWL_STYLE,WS_VISIBLE | WS_CHILD | WS_TABSTOP);
    }
    return TRUE;
}

void create_database_view(HWND hwnd,DatabaseViewModel * model){
    HINSTANCE hinst = App->hInstance;

    for(int ix = 0; ix < 5; ix ++){
        create_widget(hwnd,hinst,&query_row[ix]);
    }

    model->hwndCursorText       = query_row[0].hwnd;
    model->hwndPatternText      = query_row[1].hwnd;
    model->hwndCountText        = query_row[2].hwnd;
    model->hwndSearchButton     = query_row[3].hwnd;
    model->hwndNextSearchButton = query_row[4].hwnd;

    EnumChildWindows(hwnd,enumChildProc,0);
}

/** search given database with parameters. */
void search_database(HWND hwnd,DatabaseViewModel * model){
    BOOL result1 = TRUE;
    BOOL result2 = TRUE;
    TreeNode * node = model->databaseNode;
    node->database->cursor = GetDlgItemInt(hwnd,DB_CTRL_CURSOR,&result1,FALSE);
    node->database->page_size = GetDlgItemInt(hwnd,DB_CTRL_COUNT,&result2,FALSE);
    GetDlgItemText(hwnd,DB_CTRL_PATTERN,node->database->pattern,255);

    if(result1 && result2){
        s_db_get_data(model->databaseNode,
                node->database->last,
                node->database->pattern,
                node->database->page_size);
    }
}

LRESULT CALLBACK DatabaseViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    DatabaseViewModel * model = (DatabaseViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
        case WM_CREATE:{
            model = (DatabaseViewModel*)calloc(1,sizeof(DatabaseViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);
            model->toolbar = buildDatabaseToolBar(hwnd);

            create_database_view(hwnd,model);
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
                        case TB_CMD_DELETE_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_DELETE_DATA_ITEM);
                            break;
                        case TB_CMD_ADD_DATA:
                            lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_ADD_DATA_ITEM);
                            break;
                    }
                }
                break;
            }

            break;
        }
        case WM_COMMAND:{
            BOOL result1 = TRUE;
            BOOL result2 = TRUE;
            switch(LOWORD(wParam)){
                case DB_CTRL_NEXT_SEARCH:{
                    TreeNode * node = model->databaseNode;
                    node->database->cursor = GetDlgItemInt(hwnd,DB_CTRL_CURSOR,&result1,FALSE);
                    node->database->page_size = GetDlgItemInt(hwnd,DB_CTRL_COUNT,&result2,FALSE);
                    GetDlgItemText(hwnd,DB_CTRL_PATTERN,node->database->pattern,255);

                    if(result1 && result2){
                        node->database->last = node->database->cursor;
                        s_db_get_data(model->databaseNode,
                                node->database->cursor,
                                node->database->pattern,
                                node->database->page_size);
                    }

                    break;
                }

                case DB_CTRL_SEARCH:{
                    search_database(hwnd,model);
                    break;
                }

                case TB_CMD_DELETE_DATA:{
                    int result = DumpMessage(0x0008);
                    if(result == IDYES){
                        TreeNode * node = model->databaseNode;
                        s_db_flushdb(node);
                        search_database(hwnd,model);
                    }
                    break;
                }

                case TB_CMD_ADD_DATA:{
                    entry *ety = (entry *) calloc(1,sizeof(entry));
                    TreeNode * node = model->databaseNode;
                    DialogBoxParam (App->hInstance,MAKEINTRESOURCE (IDD_ADD_ENTRY),hwnd,(DLGPROC)entryDlgProc,(LPARAM)ety);

                    if(ety->key != NULL){
                        s_add_string(node,
                                ety->key,
                                ety->key_length,
                                ety->string_data->data,
                                ety->string_data->length);

                        s_db_get_data(node,
                                node->database->cursor,
                                node->database->pattern,
                                node->database->page_size);
                    }
                    break;
                }
            }
            break;
        }

        case WM_DT:{
            TreeNode * datanode = (TreeNode *) lParam;
            model->databaseNode = datanode;

            SetDlgItemText(hwnd,DB_CTRL_PATTERN,datanode->database->pattern);
            SetDlgItemInt(hwnd,DB_CTRL_CURSOR,datanode->database->cursor,FALSE);
            SetDlgItemInt(hwnd,DB_CTRL_COUNT,datanode->database->page_size,FALSE);
            break;
        }

        case WM_DTT:{
            TreeNode * datanode = (TreeNode *) wParam;
            SetDlgItemInt(hwnd,DB_CTRL_CURSOR,datanode->database->cursor,FALSE);
            break;
        }

        case WM_SIZE:{
            GetClientRect(hwnd,&rect);
            MoveWindow(model->toolbar,0,0,rect.right-rect.left,28,TRUE);
            arrange_widgets(hwnd,query_row,5);
            break;
        }
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

#define NOTE_COUNT 181
typedef struct note_table{
    int          noteid;
    const char * name;
    char       * message;
}NoteTable;

NoteTable notes[NOTE_COUNT]={
    {IDS_NOTE_SERVER_REDIS_VERSION ,"redis_version",NULL},
    {IDS_NOTE_SERVER_REDIS_GIT_SHA1 ,"redis_git_sha1",NULL},
    {IDS_NOTE_SERVER_REDIS_GIT_DIRTY ,"redis_git_dirty",NULL},
    {IDS_NOTE_SERVER_REDIS_BUILD_ID ,"redis_build_id",NULL},
    {IDS_NOTE_SERVER_REDIS_MODE ,"redis_mode",NULL},
    {IDS_NOTE_SERVER_OS ,"os",NULL},
    {IDS_NOTE_SERVER_ARCH_BITS ,"arch_bits",NULL},
    {IDS_NOTE_SERVER_MULTIPLEXING_API ,"multiplexing_api",NULL},
    {IDS_NOTE_SERVER_ATOMICVAR_API ,"atomicvar_api",NULL},
    {IDS_NOTE_SERVER_GCC_VERSION ,"gcc_version",NULL},
    {IDS_NOTE_SERVER_PROCESS_ID ,"process_id",NULL},
    {IDS_NOTE_SERVER_PROCESS_SUPERVISED ,"process_supervised",NULL},
    {IDS_NOTE_SERVER_RUN_ID ,"run_id",NULL},
    {IDS_NOTE_SERVER_TCP_PORT ,"tcp_port",NULL},
    {IDS_NOTE_SERVER_SERVER_TIME_IN_USEC ,"server_time_in_usec",NULL},
    {IDS_NOTE_SERVER_UPTIME_IN_SECONDS ,"uptime_in_seconds",NULL},
    {IDS_NOTE_SERVER_UPTIME_IN_DAYS ,"uptime_in_days",NULL},
    {IDS_NOTE_SERVER_HZ ,"hz",NULL},
    {IDS_NOTE_SERVER_CONFIGURED_HZ ,"configured_hz",NULL},
    {IDS_NOTE_SERVER_LRU_CLOCK ,"lru_clock",NULL},
    {IDS_NOTE_SERVER_EXECUTABLE ,"executable",NULL},
    {IDS_NOTE_SERVER_CONFIG_FILE ,"config_file",NULL},
    {IDS_NOTE_CLIENT_CONNECTED_CLIENTS,"connected_clients",NULL},
    {IDS_NOTE_CLIENT_CLUSTER_CONNECTIONS,"cluster_connections",NULL},
    {IDS_NOTE_CLIENT_MAXCLIENTS,"maxclients",NULL},
    {IDS_NOTE_CLIENT_CLIENT_LONGEST_OUTPUT_LIST,"client_longest_output_list",NULL},
    {IDS_NOTE_CLIENT_CLIENT_BIGGEST_INPUT_BUF,"client_biggest_input_buf",NULL},
    {IDS_NOTE_CLIENT_BLOCKED_CLIENTS,"blocked_clients",NULL},
    {IDS_NOTE_CLIENT_TRACKING_CLIENTS,"tracking_clients",NULL},
    {IDS_NOTE_CLIENT_CLIENTS_IN_TIMEOUT_TABLE,"clients_in_timeout_table",NULL},
    {IDS_NOTE_CLIENT_IO_THREADS_ACTIVE,"io_threads_active",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY,"used_memory",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_HUMAN,"used_memory_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_RSS,"used_memory_rss",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_RSS_HUMAN,"used_memory_rss_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_PEAK,"used_memory_peak",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_PEAK_HUMAN,"used_memory_peak_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_PEAK_PERC,"used_memory_peak_perc",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_OVERHEAD,"used_memory_overhead",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_STARTUP,"used_memory_startup",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_DATASET,"used_memory_dataset",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_DATASET_PERC,"used_memory_dataset_perc",NULL},
    {IDS_NOTE_MEMORY_TOTAL_SYSTEM_MEMORY,"total_system_memory",NULL},
    {IDS_NOTE_MEMORY_TOTAL_SYSTEM_MEMORY_HUMAN,"total_system_memory_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_LUA,"used_memory_lua",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_LUA_HUMAN,"used_memory_lua_human",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_SCRIPTS,"used_memory_scripts",NULL},
    {IDS_NOTE_MEMORY_USED_MEMORY_SCRIPTS_HUMAN,"used_memory_scripts_human",NULL},
    {IDS_NOTE_MEMORY_MAXMEMORY,"maxmemory",NULL},
    {IDS_NOTE_MEMORY_MAXMEMORY_HUMAN,"maxmemory_human",NULL},
    {IDS_NOTE_MEMORY_MAXMEMORY_POLICY,"maxmemory_policy",NULL},
    {IDS_NOTE_MEMORY_MEM_FRAGMENTATION_RATIO,"mem_fragmentation_ratio",NULL},
    {IDS_NOTE_MEMORY_MEM_FRAGMENTATION_BYTES,"mem_fragmentation_bytes",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_FRAG_RATIO,"allocator_frag_ratio",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_FRAG_BYTES,"allocator_frag_bytes",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_RSS_RATIO,"allocator_rss_ratio",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_RSS_BYTES,"allocator_rss_bytes",NULL},
    {IDS_NOTE_MEMORY_RSS_OVERHEAD_RATIO,"rss_overhead_ratio",NULL},
    {IDS_NOTE_MEMORY_RSS_OVERHEAD_BYTES,"rss_overhead_bytes",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_ALLOCATED,"allocator_allocated",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_ACTIVE,"allocator_active",NULL},
    {IDS_NOTE_MEMORY_ALLOCATOR_RESIDENT,"allocator_resident",NULL},
    {IDS_NOTE_MEMORY_MEM_NOT_COUNTED_FOR_EVICT,"mem_not_counted_for_evict",NULL},
    {IDS_NOTE_MEMORY_MEM_CLIENTS_NORMAL,"mem_clients_normal",NULL},
    {IDS_NOTE_MEMORY_MEM_CLIENTS_SLAVES,"mem_clients_slaves",NULL},
    {IDS_NOTE_MEMORY_MEM_AOF_BUFFER,"mem_aof_buffer",NULL},
    {IDS_NOTE_MEMORY_MEM_REPLICATION_BACKLOG,"mem_replication_backlog",NULL},
    {IDS_NOTE_MEMORY_MEM_TOTAL_REPLICATION_BUFFERS,"mem_total_replication_buffers",NULL},
    {IDS_NOTE_MEMORY_MEM_ALLOCATOR,"mem_allocator",NULL},
    {IDS_NOTE_MEMORY_ACTIVE_DEFRAG_RUNNING,"active_defrag_running",NULL},
    {IDS_NOTE_MEMORY_LAZYFREE_PENDING_OBJECTS,"lazyfree_pending_objects",NULL},
    {HDS_NOTE_STAT_TOTAL_CONNECTIONS_RECEIVED,"total_connections_received",NULL}  ,  
    {HDS_NOTE_STAT_TOTAL_COMMANDS_PROCESSED,"total_commands_processed",NULL}    ,  
    {HDS_NOTE_STAT_INSTANTANEOUS_OPS_PER_SEC,"instantaneous_ops_per_sec",NULL}   ,  
    {HDS_NOTE_STAT_TOTAL_NET_INPUT_BYTES,"total_net_input_bytes",NULL}       ,  
    {HDS_NOTE_STAT_TOTAL_NET_OUTPUT_BYTES,"total_net_output_bytes",NULL}      ,  
    {HDS_NOTE_STAT_INSTANTANEOUS_INPUT_KBPS,"instantaneous_input_kbps",NULL}    ,  
    {HDS_NOTE_STAT_INSTANTANEOUS_OUTPUT_KBPS,"instantaneous_output_kbps",NULL}   ,  
    {HDS_NOTE_STAT_REJECTED_CONNECTIONS,"rejected_connections",NULL}        ,  
    {HDS_NOTE_STAT_SYNC_FULL,"sync_full",NULL}                   ,  
    {HDS_NOTE_STAT_SYNC_PARTIAL_OK,"sync_partial_ok",NULL}             ,  
    {HDS_NOTE_STAT_SYNC_PARTIAL_ERR,"sync_partial_err",NULL}            ,  
    {HDS_NOTE_STAT_EXPIRED_KEYS,"expired_keys",NULL}                ,  
    {HDS_NOTE_STAT_EXPIRED_STALE_PERC,"expired_stale_perc",NULL}          ,  
    {HDS_NOTE_STAT_EXPIRED_TIME_CAP_REACHED_COUNT,"expired_time_cap_reached_count",NULL},
    {HDS_NOTE_STAT_EXPIRE_CYCLE_CPU_MILLISECONDS,"expire_cycle_cpu_milliseconds",NULL} ,
    {HDS_NOTE_STAT_EVICTED_KEYS,"evicted_keys",NULL}                ,  
    {HDS_NOTE_STAT_TOTAL_EVICTION_EXCEEDED_TIME,"total_eviction_exceeded_time",NULL},  
    {HDS_NOTE_STAT_CURRENT_EVICTION_EXCEEDED_TIME,"current_eviction_exceeded_time",NULL},
    {HDS_NOTE_STAT_KEYSPACE_HITS,"keyspace_hits",NULL}               ,  
    {HDS_NOTE_STAT_KEYSPACE_MISSES,"keyspace_misses",NULL}             ,  
    {HDS_NOTE_STAT_PUBSUB_CHANNELS,"pubsub_channels",NULL}             ,  
    {HDS_NOTE_STAT_PUBSUB_PATTERNS,"pubsub_patterns",NULL}             ,  
    {HDS_NOTE_STAT_LATEST_FORK_USEC,"latest_fork_usec",NULL}            ,  
    {HDS_NOTE_STAT_TOTAL_FORKS,"total_forks",NULL}                 ,  
    {HDS_NOTE_STAT_MIGRATE_CACHED_SOCKETS,"migrate_cached_sockets",NULL}      ,  
    {HDS_NOTE_STAT_SLAVE_EXPIRES_TRACKED_KEYS,"slave_expires_tracked_keys",NULL}  ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_HITS,"active_defrag_hits",NULL}          ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_MISSES,"active_defrag_misses",NULL}        ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_KEY_HITS,"active_defrag_key_hits",NULL}      ,  
    {HDS_NOTE_STAT_ACTIVE_DEFRAG_KEY_MISSES,"active_defrag_key_misses",NULL}    ,  
    {HDS_NOTE_STAT_TOTAL_ACTIVE_DEFRAG_TIME,"total_active_defrag_time",NULL}    ,  
    {HDS_NOTE_STAT_CURRENT_ACTIVE_DEFRAG_TIME,"current_active_defrag_time",NULL}  ,  
    {HDS_NOTE_STAT_TRACKING_TOTAL_KEYS,"tracking_total_keys",NULL}         ,  
    {HDS_NOTE_STAT_TRACKING_TOTAL_ITEMS,"tracking_total_items",NULL}        ,  
    {HDS_NOTE_STAT_TRACKING_TOTAL_PREFIXES,"tracking_total_prefixes",NULL}     ,  
    {HDS_NOTE_STAT_UNEXPECTED_ERROR_REPLIES,"unexpected_error_replies",NULL}    ,  
    {HDS_NOTE_STAT_TOTAL_ERROR_REPLIES,"total_error_replies",NULL}         ,  
    {HDS_NOTE_STAT_TOTAL_READS_PROCESSED,"total_reads_processed",NULL}       ,  
    {HDS_NOTE_STAT_TOTAL_WRITES_PROCESSED,"total_writes_processed",NULL}      ,  
    {HDS_NOTE_STAT_IO_THREADED_READS_PROCESSED,"io_threaded_reads_processed",NULL} ,  
    {HDS_NOTE_STAT_IO_THREADED_WRITES_PROCESSED,"io_threaded_writes_processed",NULL},
    {IDS_NOTE_PERSIST_LOADING,"loading",NULL},
    {IDS_NOTE_PERSIST_CURRENT_COW_PEAK,"current_cow_peak",NULL},
    {IDS_NOTE_PERSIST_CURRENT_COW_SIZE,"current_cow_size",NULL},
    {IDS_NOTE_PERSIST_CURRENT_FORK_PERC,"current_fork_perc",NULL},
    {IDS_NOTE_PERSIST_CURRENT_SAVE_KEYS_PROCESSED,"current_save_keys_processed",NULL},
    {IDS_NOTE_PERSIST_CURRENT_SAVE_KEYS_TOTAL,"current_save_keys_total",NULL},
    {IDS_NOTE_PERSIST_RDB_CHANGES_SINCE_LAST_SAVE,"rdb_changes_since_last_save",NULL},
    {IDS_NOTE_PERSIST_RDB_BGSAVE_IN_PROGRESS,"rdb_bgsave_in_progress",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_SAVE_TIME,"rdb_last_save_time",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_BGSAVE_STATUS,"rdb_last_bgsave_status",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_BGSAVE_TIME_SEC,"rdb_last_bgsave_time_sec",NULL},
    {IDS_NOTE_PERSIST_RDB_CURRENT_BGSAVE_TIME_SEC,"rdb_current_bgsave_time_sec",NULL},
    {IDS_NOTE_PERSIST_RDB_LAST_COW_SIZE,"rdb_last_cow_size",NULL},
    {IDS_NOTE_PERSIST_AOF_ENABLED,"aof_enabled",NULL},
    {IDS_NOTE_PERSIST_AOF_REWRITE_IN_PROGRESS,"aof_rewrite_in_progress",NULL},
    {IDS_NOTE_PERSIST_AOF_REWRITE_SCHEDULED,"aof_rewrite_scheduled",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_REWRITE_TIME_SEC,"aof_last_rewrite_time_sec",NULL},
    {IDS_NOTE_PERSIST_AOF_CURRENT_REWRITE_TIME_SEC,"aof_current_rewrite_time_sec",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_BGREWRITE_STATUS,"aof_last_bgrewrite_status",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_WRITE_STATUS,"aof_last_write_status",NULL},
    {IDS_NOTE_PERSIST_AOF_LAST_COW_SIZE,"aof_last_cow_size",NULL},
    {IDS_NOTE_PERSIST_MODULE_FORK_IN_PROGRESS,"module_fork_in_progress",NULL},
    {IDS_NOTE_PERSIST_MODULE_FORK_LAST_COW_SIZE,"module_fork_last_cow_size",NULL},
    {IDS_NOTE_PERSIST_AOF_CURRENT_SIZE,"aof_current_size",NULL},
    {IDS_NOTE_PERSIST_AOF_BASE_SIZE,"aof_base_size",NULL},
    {IDS_NOTE_PERSIST_AOF_PENDING_REWRITE,"aof_pending_rewrite",NULL},
    {IDS_NOTE_PERSIST_AOF_BUFFER_LENGTH,"aof_buffer_length",NULL},
    {IDS_NOTE_PERSIST_AOF_REWRITE_BUFFER_LENGTH,"aof_rewrite_buffer_length",NULL},
    {IDS_NOTE_PERSIST_AOF_PENDING_BIO_FSYNC,"aof_pending_bio_fsync",NULL},
    {IDS_NOTE_PERSIST_AOF_DELAYED_FSYNC,"aof_delayed_fsync",NULL},
    {IDS_NOTE_PERSIST_LOADING_START_TIME,"loading_start_time",NULL},
    {IDS_NOTE_PERSIST_LOADING_TOTAL_BYTES,"loading_total_bytes",NULL},
    {IDS_NOTE_PERSIST_LOADING_RDB_USED_MEM,"loading_rdb_used_mem",NULL},
    {IDS_NOTE_PERSIST_LOADING_LOADED_BYTES,"loading_loaded_bytes",NULL},
    {IDS_NOTE_PERSIST_LOADING_LOADED_PERC,"loading_loaded_perc",NULL},
    {IDS_NOTE_PERSIST_LOADING_ETA_SECONDS,"loading_eta_seconds",NULL},
    {IDS_NOTE_REPLICAT_ROLE,"role",NULL},
    {IDS_NOTE_REPLICAT_MASTER_FAILOVER_STATE,"master_failover_state",NULL},
    {IDS_NOTE_REPLICAT_MASTER_REPLID,"master_replid",NULL},
    {IDS_NOTE_REPLICAT_MASTER_REPLID2,"master_replid2",NULL},
    {IDS_NOTE_REPLICAT_MASTER_REPL_OFFSET,"master_repl_offset",NULL},
    {IDS_NOTE_REPLICAT_SECOND_REPL_OFFSET,"second_repl_offset",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_ACTIVE,"repl_backlog_active",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_SIZE,"repl_backlog_size",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_FIRST_BYTE_OFFSET,"repl_backlog_first_byte_offset",NULL},
    {IDS_NOTE_REPLICAT_REPL_BACKLOG_HISTLEN,"repl_backlog_histlen",NULL},
    {IDS_NOTE_REPLICAT_MASTER_HOST,"master_host",NULL},
    {IDS_NOTE_REPLICAT_MASTER_PORT,"master_port",NULL},
    {IDS_NOTE_REPLICAT_MASTER_LINK_STATUS,"master_link_status",NULL},
    {IDS_NOTE_REPLICAT_MASTER_LAST_IO_SECONDS_AGO,"master_last_io_seconds_ago",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_IN_PROGRESS,"master_sync_in_progress",NULL},
    {IDS_NOTE_REPLICAT_SLAVE_REPL_OFFSET,"slave_repl_offset",NULL},
    {IDS_NOTE_REPLICAT_SLAVE_PRIORITY,"slave_priority",NULL},
    {IDS_NOTE_REPLICAT_SLAVE_READ_ONLY,"slave_read_only",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_TOTAL_BYTES,"master_sync_total_bytes",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_READ_BYTES,"master_sync_read_bytes",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_LEFT_BYTES,"master_sync_left_bytes",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_PERC,"master_sync_perc",NULL},
    {IDS_NOTE_REPLICAT_MASTER_SYNC_LAST_IO_SECONDS_AGO,"master_sync_last_io_seconds_ago",NULL},
    {IDS_NOTE_REPLICAT_MASTER_LINK_DOWN_SINCE_SECONDS,"master_link_down_since_seconds",NULL},
    {IDS_NOTE_REPLICAT_CONNECTED_SLAVES,"connected_slaves",NULL},
    {IDS_NOTE_REPLICAT_MIN_SLAVES_GOOD_SLAVES,"min_slaves_good_slaves",NULL},
    {IDS_NOTE_CPU_USED_CPU_SYS,"used_cpu_sys",NULL},
    {IDS_NOTE_CPU_USED_CPU_USER,"used_cpu_user",NULL},
    {IDS_NOTE_CPU_USED_CPU_SYS_CHILDREN,"used_cpu_sys_children",NULL},
    {IDS_NOTE_CPU_USED_CPU_USER_CHILDREN,"used_cpu_user_children",NULL},
    {IDS_NOTE_CPU_USED_CPU_SYS_MAIN_THREAD,"used_cpu_sys_main_thread",NULL},
    {IDS_NOTE_CPU_USED_CPU_USER_MAIN_THREAD,"used_cpu_user_main_thread",NULL},
    {IDS_NOTE_CLUSTER_ENABLED,"cluster_enabled",NULL}
};

const int cids[3]={IDS_LV_COLUMN_PROPERTY,IDS_LV_COLUMN_VALUE,IDS_LV_COLUMN_NOTE};

char * find_message(const char * name){
    for(int ix = 0; ix < NOTE_COUNT; ix ++){
        if(strcmp(name,notes[ix].name) == 0){
            return notes[ix].message;
        }
    }
    return NULL;
}

/* init system view table headers */
BOOL initSystemViewHeader(HWND hWndListView) { 
    LVCOLUMN lvc;
    int iCol;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (iCol = 0; iCol < 3; iCol++){
        char * buff = (char *)calloc(1,255);
        LoadString(App->hInstance,cids[iCol],buff,255);
 
        lvc.iSubItem = iCol;
        lvc.pszText = buff;
        lvc.cx = 200;
        lvc.fmt = LVCFMT_LEFT;

        ListView_InsertColumn(hWndListView, iCol, &lvc);
        ListView_SetColumnWidth(hWndListView,iCol,(iCol+1) * 200);

        free(buff);
    }
    
    ListView_SetColumnWidth(hWndListView,0,300);
    ListView_SetColumnWidth(hWndListView,1,100);
    ListView_SetColumnWidth(hWndListView,2,600);

    return TRUE;
}

HWND buildStatToolBar(HWND parent){
    int buttonCount = 14;
    TBBUTTON tbtn[14] = {
        {(TB_DISCONNECT_BUTTON), TB_CMD_DISCONNECT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {-1}, 0, 0},
        {(0), 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
        {(1), IDM_STAT_SERVER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(2), IDM_STAT_CLIENT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDM_STAT_MEMORY, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(4), IDM_STAT_PERSISENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(5), IDM_STAT_STATS, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(6), IDM_STAT_REPLICATION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(7), IDM_STAT_CPU, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(8), IDM_STAT_CLUSTER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(9), IDM_STAT_KEYSPACE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(10), 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
        {(28), IDM_STAT_CLIENT_LIST, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_DELETE_BUTTON), TB_CMD_FLUSH_DB, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);;
}

BOOL updateConfigDataSet(HWND hwnd,KVPair kv){
    LVITEM lvI;

    lvI.pszText   = LPSTR_TEXTCALLBACK;
    lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    SendMessage(hwnd,LVM_DELETEALLITEMS,(WPARAM)NULL,(LPARAM)NULL);

    for (int index = 0; index < kv->count; index++){
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;

        lvI.pszText = kv->array[index]->key;
        ListView_InsertItem(hwnd, &lvI);

        lvI.pszText = kv->array[index]->value;
        lvI.iSubItem = 1;
        //ListView_InsertItem(hwnd, &lvI);
        SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);

        char * msg = find_message(kv->array[index]->key);
        if(msg != NULL){
            lvI.pszText = msg;
            lvI.iSubItem = 2;
            SendMessage(hwnd,LVM_SETITEM,(WPARAM)NULL,(LPARAM)&lvI);
        }
    }

    return TRUE;
}

LRESULT CALLBACK SystemViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    SystemViewModel model = (SystemViewModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
        case WM_CREATE:{
            model = (SystemViewModel)calloc(1,sizeof(struct system_view_model));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            GetClientRect (hwnd, &rect); 

            model->paramViewHwnd = CreateWindowEx(!WS_EX_CLIENTEDGE, "SysListView32", NULL,
                          WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_SHAREIMAGELISTS,
                          0, 0,0,0,
                          hwnd, NULL, App->hInstance, NULL);
            model->toolBar = buildStatToolBar(hwnd);

            ListView_SetExtendedListViewStyle(model->paramViewHwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            initSystemViewHeader(model->paramViewHwnd);

            for(int ix = 0; ix < NOTE_COUNT; ix ++){
               notes[ix].message = (char *)calloc(1,500);
               LoadString(App->hInstance,notes[ix].noteid,notes[ix].message,500);
            }

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
                        case TB_CMD_FLUSH_DB:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_FLUSH_ALL);break;}
                        case TB_CMD_DISCONNECT:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_DISCONNECT);break;}
                        case IDM_STAT_SERVER:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_SERVER);break;};
                        case IDM_STAT_CLIENT:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_CLIENTS);break;}
                        case IDM_STAT_MEMORY:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_MEMORY);break;}
                        case IDM_STAT_PERSISENCE:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_PERSISTENCE);break;}
                        case IDM_STAT_STATS:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_STATS);break;}
                        case IDM_STAT_REPLICATION:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_REPLICATION);break;}
                        case IDM_STAT_CPU:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_CPU);break;}
                        case IDM_STAT_CLUSTER:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_CLUSTER);break;}
                        case IDM_STAT_KEYSPACE:{lpttt->lpszText = MAKEINTRESOURCE(IDS_TB_SYS_KEYSPACE);break;}
                    }
                }
                break;
            }

            break;
        }
        case WM_COMMAND:{
            int cmd = LOWORD(wParam);
            if(cmd == TB_CMD_FLUSH_DB){
                int result = DumpMessage(0x0006);
                if(result == IDYES){
                    ;// TODO handle flushall command
                }
            }
            else if(cmd == TB_CMD_DISCONNECT){
                SendMessage(App->mainWindowHwnd,WM_REMOVE_CONNECTION,(WPARAM)App->activeHost,(LPARAM)NULL);
            }else if(cmd == IDM_STAT_CLIENT_LIST){
				// TODO add client list information.
			}
            else{
                stat_command(hwnd,LOWORD (wParam));
            }
            return 0;
        }

        case WM_DT:{
            RedisReply data     = (RedisReply) wParam;
            KVPair kp = parseKVPair(data->bulk->content);
            updateConfigDataSet(model->paramViewHwnd,kp);
            break;
        }

        case WM_SIZE:{
            GetClientRect(hwnd,&rect);
            MoveWindow(hwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
            MoveWindow(model->paramViewHwnd,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
            break;
        }
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

void stat_command(HWND hwnd,int cmd){
    const char * statcmd = "stats";
    switch (cmd){
        case IDM_STAT_SERVER:{statcmd = "server";break;}
        case IDM_STAT_CLIENT:{statcmd = "clients";break;}
        case IDM_STAT_MEMORY:{statcmd = "memory";break;}
        case IDM_STAT_PERSISENCE:{statcmd = "persistence";break;}
        case IDM_STAT_STATS:{statcmd = "stats";break;}
        case IDM_STAT_REPLICATION:{statcmd = "replication";break;}
        case IDM_STAT_CPU:{statcmd = "cpu";break;}
        case IDM_STAT_CLUSTER:{statcmd = "cluster";break;}
        case IDM_STAT_KEYSPACE:{statcmd = "keyspace";break;}
    }

    s_db_info_stats(App->activeHost,statcmd);
};

void init_systemview(HINSTANCE hInstance){
    WNDCLASSEX stringViewClass;

    stringViewClass.cbSize        = sizeof(WNDCLASSEX);
    stringViewClass.style         = 0;
    stringViewClass.lpfnWndProc   = SystemViewWndProc;
    stringViewClass.cbClsExtra    = 0;
    stringViewClass.cbWndExtra    = 0;
    stringViewClass.hInstance     = hInstance;
    stringViewClass.hIcon         = NULL;
    stringViewClass.hCursor       = NULL;
    stringViewClass.hbrBackground = resource->brush; 
    stringViewClass.lpszMenuName  = 0;
    stringViewClass.lpszClassName = SYSTEM_VIEW_CLASS;
    stringViewClass.hIconSm       = NULL;
    RegisterClassEx(&stringViewClass);
}

void SetDialogIcon(HWND hWnd, int idi) {
    HICON hIcon = LoadIcon((HINSTANCE) GetWindowLongPtr(hWnd, GWLP_HINSTANCE),MAKEINTRESOURCE(idi));
    SendMessage(hWnd, WM_SETICON, ICON_BIG,  (LPARAM)hIcon);
    SendMessage(hWnd, WM_SETICON, ICON_SMALL,(LPARAM)hIcon);
}

void MoveToScreenCenter(HWND hwnd){
    RECT rt;
    int cx,cy;

    cx = GetSystemMetrics(SM_CXSCREEN);
    cy = GetSystemMetrics(SM_CYSCREEN);
    GetWindowRect(hwnd,&rt);
    MoveWindow(hwnd,(cx-rt.right+rt.left)>>1,(cy-rt.bottom+rt.top)>>1,rt.right-rt.left,rt.bottom-rt.top,TRUE);
}

BOOL CALLBACK SetPreferenceProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_CONFIG_DONE:{
                    BOOL result = FALSE;
                    int val = GetDlgItemInt(hwnd,IDC_PREFERENCE_SCAN_COUNT,&result,FALSE);
                    if(result){
                        preference->db_scan_default = val;
                    }

                    preference->log_network_traffic = IsDlgButtonChecked(hwnd,87);

                    save_config();
                    EndDialog (hwnd, 0);
                    break;
                };

                case ID_CONFIG_CANCEL:{
                    EndDialog (hwnd, 0);
                    break;
                }
            }
        break;

        case WM_INITDIALOG:{
            SetDlgItemInt(hwnd,IDC_PREFERENCE_SCAN_COUNT,preference->db_scan_default,FALSE);
            Button_SetCheck(GetDlgItem(hwnd,87),
                    preference->log_network_traffic);

            MoveToScreenCenter(hwnd);
            break;
        }
    }

    return FALSE;
}

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
        case WM_INITDIALOG:{
            MoveToScreenCenter(hwnd);
            HICON hIcon = LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),MAKEINTRESOURCE(IDI_MAIN));
            SendDlgItemMessage(hwnd, 8888, STM_SETICON,(WPARAM) hIcon, 0);
            break;
        }

        case WM_COMMAND:{
            switch (LOWORD (wParam)){
                case IDC_ABOUT_OK:{
                    EndDialog (hwnd, 0);
                    break;
                }
            }
            break;
        }

        case WM_CLOSE:
            EndDialog(hwnd,0);
        break;
    }

    return FALSE;
}
