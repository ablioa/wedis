#include "main.h"
#include "entry.h"

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

BOOL CALLBACK entryDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            HWND chwnd = GetDlgItem(hwnd,IDC_DATATYPE);
            AddItems(chwnd);
            MoveToScreenCenter(hwnd);
            /////////////////////////
            


                CreateWindowEx(0, WINDOW_CONTAINER,NULL, 
                    WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                    5,20+rand() % 50,200,200,hwnd, (HMENU)0, App->hInstance, 0);
            ///////////////////////////
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case IDOK:{
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
                        char buff[255] = {0};
                        sprintf(buff,"select: %d",ix);
                        SetWindowText(hwnd,buff);
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

    stringViewClass.lpszClassName = WINDOW_CONTAINER;
    stringViewClass.lpfnWndProc   = stringEditorWndProc;
    stringViewClass.hInstance     = hInstance;

    RegisterClassEx(&stringViewClass);
}
