#include "main.h"


HINSTANCE hInstance;
TCHAR     szFrameClass[] = TEXT ("MdiFrame") ;

BOOL initApplication(){
	HANDLE hmod = LoadLibrary("srcview.dll");
    if (hmod==NULL){
    	MessageBox(NULL,"The Scintilla DLL could not be loaded.","Error loading Scintilla",MB_OK | MB_ICONERROR);
		return FALSE;
    }

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow){
    if(!initApplication()){
	    return 0;
	}

	hInstance = hInst;

	WNDCLASSEX  _class;
	_class.cbSize = sizeof (WNDCLASSEX);
	_class.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	_class.lpfnWndProc=MainWndProc;
	_class.lpszClassName = szFrameClass;
	_class.hInstance = hInst;
	_class.hIcon = 0;
	_class.hIconSm = 0;
	_class.lpszMenuName = MAKEINTRESOURCE (ID_MAIN);
	_class.cbClsExtra = 0;
	_class.cbWndExtra = 0;
	_class.hbrBackground = (HBRUSH) GetStockObject (COLOR_BTNFACE + 1);
	_class.hCursor = LoadCursor (0, IDC_ARROW);

    if (!RegisterClassEx (&_class)){
         MessageBox (NULL, TEXT ("This program requires Windows NT!"),
                     TEXT("appName"), MB_ICONERROR) ;
         return 0 ;
    }

	HWND hwndFrame = CreateWindowEx(WS_EX_LEFT,szFrameClass, TEXT ("MDI Demonstration"),
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
							   CW_USEDEFAULT, 0,
							   CW_USEDEFAULT, 0,
                               NULL,NULL, hInst, NULL) ;

     ShowWindow (hwndFrame, cmdShow) ;
     UpdateWindow (hwndFrame) ;

	HANDLE hAccel = LoadAccelerators(hInst,MAKEINTRESOURCE(IDA_RUN));

	MSG  msg;
	while (GetMessage (&msg, 0, 0, 0)){
		if(!TranslateAccelerator(msg.hwnd,(HACCEL)hAccel,&msg)){
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
    }

    return msg.wParam;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    //Controller * pCtrl = WinGetLong<Controller *> (hwnd);

    Controller * pCtrl = (Controller *)GetWindowLong(hwnd,GWL_USERDATA);
	RECT rt;
	POINT pos;
    switch (message){
		case WM_NOTIFY:{
			switch (((LPNMHDR) lParam)->code) {
				case TVN_BEGINDRAG:
					MessageBox(hwnd,"wwwwww","www",MB_OK);
				break;
			}

			return 0;
		}
        case WM_CREATE:
            pCtrl = new Controller (hwnd, reinterpret_cast<CREATESTRUCT *> (lParam));
            SetWindowLong(hwnd,GWL_USERDATA,(LONG)pCtrl);
            pCtrl->LoadSourceView();

            return 0;
        case WM_SIZE:
            pCtrl->Size(LOWORD(lParam), HIWORD(lParam));
            pCtrl->Paint();
            return 0;

        case WM_PAINT:
            pCtrl->Paint();
            return 0;

        case WM_COMMAND:
                pCtrl->Command (LOWORD (wParam));
            return 0;

        case WM_DESTROY:
            //WinSetLong<Controller *> (hwnd, 0);
            //delete pCtrl;

            PostQuitMessage(0);
        return 0;

        case WM_RBUTTONDOWN:
                GetWindowRect(hwnd,&rt);
                pos.x= LOWORD(lParam);
                pos.y = HIWORD(lParam);
                TrackPopupMenu(pCtrl->hDev,TPM_RIGHTALIGN,rt.left+pos.x,rt.top+pos.y,0,hwnd,NULL);
        return 0;
    }

    return ::DefWindowProc (hwnd, message, wParam, lParam);
}


