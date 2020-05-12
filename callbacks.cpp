#include "callbacks.h"

int tmp_break;
int dest_line;

LRESULT CALLBACK ColorProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam)
{
		//HWND hwnd;
	HDC	hdc;
	PAINTSTRUCT ps;
	HWND hInst;
	CHOOSECOLOR *lpChooseColor;
	COLORREF		lpCust[16]={0xff};
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
				case IDC_MORE_COLOR:
					lpChooseColor = new CHOOSECOLOR();
					ZeroMemory(lpChooseColor,sizeof(CHOOSECOLOR));
					lpChooseColor->hInstance = (HWND)GetWindowLong(hWin,GWL_HINSTANCE);
					lpChooseColor->lStructSize = sizeof(CHOOSECOLOR);
					lpChooseColor->hwndOwner = hWin;
					lpChooseColor->lpCustColors=lpCust;
					ChooseColor(lpChooseColor);
					delete lpChooseColor;
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

LRESULT CALLBACK SetBrkProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam)
{

//	char buff[MAX_PATH];
	switch(msg)
	{
		case WM_INITDIALOG:

		break;

		case WM_COMMAND:
			switch(wParam){
				case IDOK:
					tmp_break=GetDlgItemInt(hWin,IDC_BRKVALUE,NULL,FALSE);
					EndDialog(hWin,0);
				break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hWin,0);
		break;
	}

	return 0;
}
