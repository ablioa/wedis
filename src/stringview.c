#include "stringview.h"

#include <windowsx.h>

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

static HWND build_toolbar(HWND parent){
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
            model->toolBar = build_toolbar(hwnd);
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

