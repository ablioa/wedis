#include "main.h"
#include "entry.h"

widget key_row[3] = {
    {NULL,{5, 5,-1,20},WC_EDIT,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,(HMENU)9},
    {NULL,{110,5,40,20},WC_BUTTON,"...",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)3},
    {NULL,{160,5,60,100},WC_COMBOBOX,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWN,(HMENU)IDC_DATATYPE}
};

widget entry_pane[5]={
    {NULL,{5,30,40,20},WDS_PANE,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)1},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1},
};

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

LRESULT CALLBACK stringEditorWndProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

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
                MoveWindow(wgt->hwnd,pos.left,pos.top+ix * 10,pos.width,pos.height,FALSE);
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
    stringViewClass.lpfnWndProc   = stringEditorWndProc;
    stringViewClass.hInstance     = hInstance;

    RegisterClassEx(&stringViewClass);
}
