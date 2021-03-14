#include "callbacks.h"

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
				case IDC_CONFIG_DONE:
					EndDialog (hwnd, 0);
				break;

				case ID_CONFIG_CANCEL:{
					EndDialog (hwnd, 0);
					break;
				}
			}
		break;

		case WM_INITDIALOG:{
			/** TODO use universal font */
			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(GetDlgItem(hwnd,IDC_CONFIG_DONE), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,ID_CONFIG_CANCEL), WM_SETFONT, (WPARAM)hfont0, FALSE);

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

			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(GetDlgItem(hwnd,IDC_ABOUT_OK), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_URL), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_NAME), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_DESCRIPTION), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_IMAGE), WM_SETFONT, (WPARAM)hfont0, FALSE);
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

