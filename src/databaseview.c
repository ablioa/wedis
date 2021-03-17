#include "databaseview.h"

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
    hashViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    hashViewClass.lpszMenuName  = 0;
    hashViewClass.lpszClassName = DATABASE_VIEW_CLASS;
    hashViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    
    RegisterClassEx(&hashViewClass);
}

BOOL CALLBACK enumChildProc(HWND hwnd,LPARAM lParam){
    SendMessage(hwnd, WM_SETFONT, (WPARAM)(resource->ctrlFont), FALSE);
    return TRUE;
}

void create_database_view(HWND hwnd,DatabaseViewModel * model){
    HINSTANCE hinst = App->hInstance;

    model->hwndCursorText = CreateWindowEx(0, WC_EDIT, ("0"), 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
            5, 28, 80, 20, hwnd, (HMENU)DB_CTRL_CURSOR, hinst, 0); 
    
    model->hwndPatternText = CreateWindowEx(0, WC_EDIT, ("*"), 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
            90, 28, 80, 20, hwnd, (HMENU)DB_CTRL_PATTERN, hinst, 0);    

    model->hwndCountText = CreateWindowEx(0, WC_EDIT, ("10"), 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
            175, 28, 80, 20, hwnd, (HMENU)DB_CTRL_COUNT, hinst, 0); 

    model->hwndSearchButton = CreateWindowEx(0, WC_BUTTON, ("Scan"), 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
            260, 28, 60, 20, hwnd, (HMENU)DB_CTRL_SEARCH, hinst, 0); 

    model->hwndNextSearchButton = CreateWindowEx(0, WC_BUTTON, ("Next"), 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
            325, 28, 60, 20, hwnd, (HMENU)DB_CTRL_NEXT_SEARCH, hinst, 0); 

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
    }else{
        MessageBox(hwnd,"Input Parameter Invalid!","Wedis",MB_OK);
    }
}

LRESULT CALLBACK DatabaseViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rect;
    DatabaseViewModel * model = (DatabaseViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(message){
        case WM_CREATE:{
            model = (DatabaseViewModel*)calloc(1,sizeof(DatabaseViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);
            model->toolBar = buildDatabaseToolBar(hwnd);

            create_database_view(hwnd,model);
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

                case 1:{
                    int result = MessageBox(hwnd,"flushdb will remove all data in the database,continue?","wedis",MB_YESNOCANCEL|MB_ICONASTERISK);
                    if(result == IDYES){
                        TreeNode * node = model->databaseNode;
                        s_db_flushdb(node);
                        search_database(hwnd,model);
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
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
            break;
        }
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

HWND buildDatabaseToolBar(HWND parent){
    int buttonCount = 1;
    TBBUTTON tbtn[1] = {
        {(TB_DELETE_BUTTON), 1, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);
}
