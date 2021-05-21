#include "callbacks.h"

#include "json.h"

#include <windowsx.h>

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

