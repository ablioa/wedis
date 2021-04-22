#include "main.h"
#include "entry.h"

widget key_row[3] = {
    {NULL,{5, 5,-1,20},WC_EDIT,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,(HMENU)9},
    {NULL,{110,5,40,20},WC_BUTTON,"...",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)3},
    {NULL,{160,5,60,100},WC_COMBOBOX,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWN,(HMENU)IDC_DATATYPE}
};

widget entry_pane[5]={
    {NULL,{5,30,40,20},WDS_PANE,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)1,stringEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,listEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,hashEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,setEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,zetEntryEditorWndProc}
};

#define IMPORT_STRING_DATA_BUTTON 9999
#define IMPORT_STRING_DATA_EDITOR 8888

/** string editor layout */
widget string_editor_pane[2]={
    {NULL,{0,0,200,20},TOOLBARCLASSNAME,"",WS_VISIBLE | WS_CHILD | TBSTYLE_TOOLTIPS|TBSTYLE_FLAT,(HMENU)IMPORT_STRING_DATA_BUTTON},
    {NULL,{0,25,100,100},WC_EDIT,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,(HMENU)IMPORT_STRING_DATA_EDITOR}
};

widget string_list_pane[1]={
    {NULL,{0,0,200,200},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT|LVS_SHAREIMAGELISTS,(HMENU)9}
};

widget string_hash_pane[1]={
    {NULL,{0,0,0,0},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_EDITLABELS,(HMENU)9}
};

widget string_set_pane[1]={
    {NULL,{0,0,0,0},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,(HMENU)9}
};

widget string_zset_pane[1]={
    {NULL,{0,0,0,0},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,(HMENU)9}
};


const char * list_entry_column[2] = {"index","item"};
const char * set_entry_column[1]  = {"item"};
const char * hash_entry_column[2] = {"key","value"};
const char * zset_entry_column[2] = {"score","item"};

BOOL init_list_column_name(HWND hWndListView,const char ** item,const int columns) {
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int i = 0; i < columns; i++){
        char * buff = calloc(1,255);
        strcpy(buff,item[i]);

        lvc.pszText  = buff;
        lvc.cx       = 100;
        lvc.iSubItem = i;
        lvc.fmt      = LVCFMT_LEFT;
        ListView_InsertColumn(hWndListView, i, &lvc);
    }
    
    return TRUE;
}

static void set_default_style(HWND hwnd){
    EnumChildWindows(hwnd,enumChildProc,0);
}

static void AddItems(HWND hWndComboBox){
    const char * types[5]= {"- string","- list","- hash","- set","- zset"};
    for (int k = 0; k <= 4; k++){
        SendMessage(hWndComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) types[k]); 
    }
      
    SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

LRESULT CALLBACK default_entry_proc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

int buttonCount = 1;
TBBUTTON string_toolbar_buttons[1] = {
    {(TB_ADD_BUTTON), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
};

LRESULT CALLBACK stringEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            GetClientRect(hwnd,&rect);
            for(int ix = 0; ix < 2; ix ++){
                widget * wgt = &string_editor_pane[ix];
                create_widget(hwnd,App->hInstance,wgt);
            }
        
            HWND tb = string_editor_pane[0].hwnd;
            SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) resource->icons);
            SendMessage(tb, TB_ADDBUTTONS,  (WPARAM)buttonCount,(LPARAM)string_toolbar_buttons);
            SendMessage(tb, TB_AUTOSIZE, 0, 0);

            MoveWindow(string_editor_pane[0].hwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,FALSE);
            MoveWindow(string_editor_pane[1].hwnd,0,28,rect.right-rect.left,rect.bottom-rect.top-28,FALSE);
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case TB_CMD_ADD_DATA:{
                    MessageBox(hwnd,"导入文本数据","Title",MB_OK);  
                    break;
                }
            }

            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK listEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_list_pane);
            HWND list_hwnd = string_list_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(list_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(list_hwnd,list_entry_column,2);

            GetClientRect(hwnd,&rect);
            MoveWindow(list_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK setEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_set_pane);
            HWND widget_hwnd = string_set_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(widget_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(widget_hwnd,set_entry_column,1);

            GetClientRect(hwnd,&rect);
            MoveWindow(widget_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK zetEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_zset_pane);
            HWND widget_hwnd = string_zset_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(widget_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(widget_hwnd,zset_entry_column,2);

            GetClientRect(hwnd,&rect);
            MoveWindow(widget_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK hashEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_hash_pane);
            HWND widget_hwnd = string_hash_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(widget_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(widget_hwnd,hash_entry_column,2);

            GetClientRect(hwnd,&rect);
            MoveWindow(widget_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK stringEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                /** import file as data */
                case 9999:{
                    MessageBox(hwnd,"hello,world","Title",MB_OK);  
                    break;
                }
            }

            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/**
 * switch editor pane among difference data type.
 */ 
static void switch_editor_pane(int index){
    for(int ix = 0; ix < 5;ix ++){
        widget wgt = entry_pane[ix];
        if(ix == index){
            ShowWindow(wgt.hwnd,SW_SHOW);
        }else{
            ShowWindow(wgt.hwnd,SW_HIDE);
        }
    }
}

BOOL CALLBACK entryDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    static entry * ety;
    switch(msg){
        case WM_INITDIALOG:{
            ety = (entry*)lParam;

            RECT hrect;
            GetClientRect(hwnd,&hrect);
            for(int ix= 0; ix < 5; ix ++){
                widget * wgt = &entry_pane[ix];
                create_widget(hwnd,App->hInstance,wgt);

                dimension pos = wgt->position;
                pos.width  = hrect.right - hrect.left - 10;
                pos.height = hrect.bottom - hrect.top - 65;
                MoveWindow(wgt->hwnd,pos.left,pos.top,pos.width,pos.height,FALSE);

                SendMessage(wgt->hwnd,WM_INIT_ENTRY,0,0);
            }

            for(int ix = 0; ix < 3; ix ++){
                create_widget(hwnd,App->hInstance,&key_row[ix]);
            }

            HWND chwnd = GetDlgItem(hwnd,IDC_DATATYPE);
            AddItems(chwnd);

            set_default_style(hwnd);
            arrange_widgets(hwnd,key_row,3);

            MoveToScreenCenter(hwnd);
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case IDOK:{
                    ety->key=(char*)calloc(255,1);
                    sprintf(ety->key,"hello,from dialog");
                    EndDialog(hwnd,0);
                    break;
                }

                case IDCANCEL:{
                    EndDialog(hwnd,0);
                    break;
                }

                case IDC_DATATYPE:{ 
                    if(HIWORD(wParam) == CBN_SELCHANGE){
                        HWND chwnd = GetDlgItem(hwnd,IDC_DATATYPE);
                        int ix = SendMessage(chwnd,CB_GETCURSEL,0,0);
                        switch_editor_pane(ix);
                    }
                    break;
                }
            }
            break;
        }

        case WM_CLOSE:{
            EndDialog(hwnd,0);
            break;
        }
    }

    return FALSE;
}

void init_editor_view(HINSTANCE hInstance){
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

    stringViewClass.lpszClassName = WDS_PANE;
    stringViewClass.lpfnWndProc   = default_entry_proc;
    stringViewClass.hInstance     = hInstance;

    RegisterClassEx(&stringViewClass);
}
