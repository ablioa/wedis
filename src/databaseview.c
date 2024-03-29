#include "databaseview.h"

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

