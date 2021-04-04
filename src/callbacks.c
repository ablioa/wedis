#include "callbacks.h"

#include "json.h"

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
                    /** TODO why json_new_item doesn't work? */

                    BOOL result = FALSE;
                    int val = GetDlgItemInt(hwnd,IDC_PREFERENCE_SCAN_COUNT,&result,FALSE);
                    if(result){
                        preference->db_scan_default = val;
                    }

                    save_all_host_config();

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
            /** TODO set preference value from config file */
            SetDlgItemInt(hwnd,IDC_PREFERENCE_SCAN_COUNT,preference->db_scan_default,FALSE);
           
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

