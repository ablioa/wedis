#include "control.h"

Controller::Controller (HWND hwnd, CREATESTRUCT * pCreate):_hwnd (hwnd)
{
	_view = new View(hwnd);
	app = new App(hwnd);
    hDev = CreatePopupMenu();

    AppendMenu(hDev,MF_STRING,IDM_SET,"ATMEL 8051");
    AppendMenu(hDev,MF_SEPARATOR,IDM_SET,"");
    AppendMenu(hDev,MF_STRING,IDM_CLEAR,"ATEML 8751");
    AppendMenu(hDev,MF_STRING,IDM_HELPa,"ATMEL AT89C52");
    AppendMenu(hDev,MF_STRING,IDM_ABOUTa,"ATMEL AT89S51");
    AppendMenu(hDev,MF_SEPARATOR,IDM_SET,"");
    AppendMenu(hDev,MF_STRING,IDM_EXITa,"ATMEL AT89C51");
}

void Controller::LoadSourceView(){
	_view->CreateView();
}

Controller::~Controller(){
    ::PostQuitMessage(0);
}

void Controller::Size (int cx, int cy){
	_view->Size();
}

void Controller::Paint (){
    PaintCanvas canvas (_hwnd);
}

void Controller::Command (int cmd){
	HINSTANCE hInst;
	char *fname;
	RECT rt;
	POINT pt;

    switch (cmd)
    {
		case IDM_FILE_OPEN:
			fname = app->GetOpenFileName();
			if(!fname)
				break;

			break;
		case IDM_FILE_SAVESOURCE:

			break;
		case IDM_FILE_DUMP:
			break;
		case IDM_PLG_DEV:
			ShellExecute(_hwnd,"open","dev.exe","","",SW_NORMAL);
			break;

		case IDM_EXIT:
			SendMessage (_hwnd, WM_CLOSE, 0, 0L);
			break;

		case IDM_VIEW_COLOR:
				hInst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
				::DialogBox (hInst,
							 MAKEINTRESOURCE (IDD_COLOR),
							 _hwnd,
							 (DLGPROC)ColorProc);
			break;

		case IDM_PROTOCAL_DEBUG:
				hInst = (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
			    DialogBox(hInst,MAKEINTRESOURCE(IDD_PROTOCAL_DEBUG),_hwnd,(DLGPROC)DebugWindowProc);
			break;

		case IDM_HELP:
			break;

		case IDM_EXE_RUN:
			break;

		case IDM_EXE_BRK:
			MessageBox(NULL,"cvcv","cvcvc",0);
			break;

		case IDM_DEV:
			GetWindowRect(_hwnd,&rt);
			GetCursorPos(&pt);
			TrackPopupMenu(hDev,TPM_RIGHTALIGN,pt.x,pt.y,0,_hwnd,NULL);
			break;

		case IDM_STAY:
				// GetWindowRect(_hwnd,&rt);
				//SetWindowPos(_hwnd,HWND_TOPMOST,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,SWP_NOMOVE);
				//SetWindowPos(_hwnd,HWND_NOTOPMOST,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,SWP_NOMOVE);
			break;

		case IDM_ABOUT:
				hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
				DialogBox (hInst,
							 MAKEINTRESOURCE (IDD_ABOUT),
							 _hwnd,
							 AboutDlgProc);
			break;
		case IDM_VIEW_FULLSCREEN:
		break;

		case IDM_VIEW_GOTOLINE:
				hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);
				::DialogBox (hInst,
							 MAKEINTRESOURCE (IDD_GOTOLINE),
							 _hwnd,
							 GoToLineDlgProc);
		break;

		case IDM_EMU_SETBREAK:
				hInst= (HINSTANCE)GetWindowLong(_hwnd,GWL_HINSTANCE);//WinGetLong<HINSTANCE> (_hwnd, GWL_HINSTANCE);
				::DialogBox (hInst,
							 MAKEINTRESOURCE (IDD_BREAKPOINT),
							 _hwnd,
							 (DLGPROC)SetBrkProc);
		break;
	}
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
					::EndDialog (hwnd, 0);
				return TRUE;
			}
			break;
    }
    return FALSE;
}

BOOL CALLBACK GoToLineDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
	int lineTogo =0 ;
	switch(message)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BTN_OK:
					GetDlgItemInt(hwnd,IDC_ADDRESS,&lineTogo,FALSE);
					::EndDialog (hwnd, 0);
				break;
			}
		break;

		case WM_INITDIALOG:

		break;
	}
	return FALSE;
}

