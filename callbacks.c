#include "callbacks.h"

LRESULT CALLBACK SetPreferenceProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam){
	HDC	hdc;
	PAINTSTRUCT ps;
	HWND hInst;
	RECT rt;
	int cx,cy;
	char			buff[MAX_PATH];

	switch(msg)
	{
		case WM_INITDIALOG:
			hInst = (HWND)GetWindowLong(hWin,GWL_HINSTANCE);
			LoadString((HINSTANCE)hInst,IDS_CODEVIEW,buff,MAX_PATH);
			SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);
			LoadString((HINSTANCE)hInst,IDS_REGVIEW,buff,MAX_PATH);
			SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);
			LoadString((HINSTANCE)hInst,IDS_DATAVIEW,buff,MAX_PATH);
			SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);
			LoadString((HINSTANCE)hInst,IDS_STACKVIEW,buff,MAX_PATH);
			SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);
			LoadString((HINSTANCE)hInst,ID_COLOR_CURLINE,buff,MAX_PATH);
			SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);
			
			LoadString((HINSTANCE)hInst,ID_COLOR_SPLITBAR,buff,MAX_PATH);
			SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);

			cx = GetSystemMetrics(SM_CXSCREEN);
			cy = GetSystemMetrics(SM_CYSCREEN);
			GetWindowRect(hWin,&rt);
			MoveWindow(hWin,(cx-rt.right+rt.left)>>1,
				(cy-rt.bottom+rt.top)>>1,
				rt.right-rt.left,rt.bottom-rt.top,TRUE);
		break;

		case WM_COMMAND:
			switch(wParam)
			{
				case IDOK:
					EndDialog(hWin,0);
				break;
				case IDCANCEL:
					EndDialog(hWin,0);
				break;
			}
			break;
			case WM_PAINT:
				hdc = BeginPaint(hWin,&ps);
				Rectangle(hdc,200,40,260,60);
				Rectangle(hdc,200,100,260,120);
				EndPaint(hWin,&ps);
			break;
		case WM_CLOSE:
			EndDialog(hWin,0);
		break;
	}

	return 0;
}

BOOL CALLBACK networkDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam){
    
	switch (message){
		case WM_INITDIALOG:{
			return TRUE;
		}
		
		case WM_COMMAND:{
			switch (LOWORD (wParam)){
				case IDC_BUTTON_OK:{
					MessageBox(hwnd,"send data","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_STRING:{
					MessageBox(hwnd,"string","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_HASH:{
					MessageBox(hwnd,"hash","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_LIST:{
					MessageBox(hwnd,"list","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_SET:{
					MessageBox(hwnd,"set","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_KEYS:{
					MessageBox(hwnd,"keys","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_SELECT:{
					MessageBox(hwnd,"select","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_ZSET:{
					MessageBox(hwnd,"zset","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_CLOSE:{
					MessageBox(hwnd,"close","title",MB_OK);
					return TRUE;
				}

				case IDC_BUTTON_EXIT:{
					EndDialog(hwnd,0);
					return TRUE;
				}
			}
			break;
		}
    }
	
    return FALSE;
}

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_INITDIALOG:

			return TRUE;

			case WM_COMMAND:
			switch (LOWORD (wParam))
			{
				case IDOK:

				case IDCANCEL:
					EndDialog (hwnd, 0);
				return TRUE;
			}
			break;
    }
    return FALSE;
}

BOOL CALLBACK SetTtlDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
	int lineTogo =0 ;
	switch(message)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BTN_OK:
					GetDlgItemInt(hwnd,IDC_ADDRESS,&lineTogo,FALSE);
					EndDialog (hwnd, 0);
				break;
			}
		break;

		case WM_INITDIALOG:

		break;
	}
	return FALSE;
}