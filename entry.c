#include "entry.h"

BOOL CALLBACK entryDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam){
	char buff[MAX_PATH];

	switch(msg){
		case WM_INITDIALOG:{
			MoveToScreenCenter(hWin);
			break;
		}

		case WM_COMMAND:
			switch(wParam){
				case IDOK:
					EndDialog(hWin,0);
				break;

				case IDCANCEL:
					EndDialog(hWin,0);
				break;
			}
			break;

		case WM_PAINT:
		break;

		case WM_CLOSE:
			EndDialog(hWin,0);
		break;
	}

	return FALSE;
}