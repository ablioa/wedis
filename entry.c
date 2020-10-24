#include "entry.h"

BOOL CALLBACK entryDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	char buff[MAX_PATH];

	switch(msg){
		case WM_INITDIALOG:{
			MoveToScreenCenter(hwnd);
			break;
		}

		case WM_COMMAND:
			switch(wParam){
				case IDOK:
					MessageBox(hwnd,"Hello,Message!!","Title",MB_OK);
					EndDialog(hwnd,0);
				break;

				case IDCANCEL:
					EndDialog(hwnd,0);
				break;
			}
			break;

		case WM_PAINT:
		break;

		case WM_CLOSE:
			EndDialog(hwnd,0);
		break;
	}

	return FALSE;
}