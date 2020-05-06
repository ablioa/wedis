#include "common.h"

App::App(HWND _hwnd):hwnd(_hwnd){
	ofn = new OPENFILENAME;
	fname = new char[MAX_PATH];

	ZeroMemory(fname,MAX_PATH);

}

App::~App(){
	delete ofn;
	delete []fname;
}

LPTSTR App::GetOpenFileName(){
	LPSTR retVal = NULL;

	ZeroMemory(ofn,sizeof(OPENFILENAME));

	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);//WinGetLong<HINSTANCE> (hwnd, GWL_HINSTANCE);
	ofn->lpstrFilter = "All Files\0*.*\0HexFiles\0*.hex\0\0";
	ofn->lpstrFile = fname;
	ofn->nMaxFile =MAX_PATH;

	ofn->Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST |
		OFN_LONGNAMES | OFN_EXPLORER |OFN_HIDEREADONLY;

	if(::GetOpenFileName(ofn)){
		retVal = fname;
	}

	return retVal;
}

LPTSTR App::GetSaveFileName(){
	LPSTR retVal = NULL;

	ZeroMemory(ofn,sizeof(OPENFILENAME));

	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);//WinGetLong<HINSTANCE> (hwnd, GWL_HINSTANCE);
	ofn->lpstrFilter = "All Files\0*.*\0HexFiles\0*.hex\0\0";
	ofn->lpstrFile = fname;
	ofn->nMaxFile =MAX_PATH;

	ofn->Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST |
		OFN_LONGNAMES | OFN_EXPLORER |OFN_HIDEREADONLY;

	if(::GetSaveFileName(ofn)){
		retVal = fname;
	}

	return retVal;
}
