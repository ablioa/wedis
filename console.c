#include "console.h"

/**
 * 命令控制窗口
 */
LRESULT CALLBACK consoleWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    HINSTANCE hinst = mainModel->hInstance;//(HINSTANCE)GetWindowLong(hwnd,GWLP_HINSTANCE);
	ConsoleView * cview = (ConsoleView *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	RECT rect;

    switch(msg){
		case WM_CREATE:{
			cview = (ConsoleView*)calloc(1,sizeof(ConsoleView));
			
			HWND logHwnd  = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | WS_VSCROLL  |ES_MULTILINE, 0, 0, 100, 100, hwnd, (HMENU)0, hinst, 0);
            HWND cmdHwnd  = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER , 0, 100, 100, 24, hwnd, (HMENU)0, hinst, 0);
            
            HFONT hLogFont   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Consolas"));
			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(logHwnd, WM_SETFONT, (WPARAM)hLogFont, FALSE);
            SendMessage(cmdHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);

			cview->logHwnd = logHwnd;
			cview->cmdHwnd = cmdHwnd;
			SetWindowLong(hwnd,GWLP_USERDATA,(LONG_PTR)cview);

			cview->defaultCmdProc = (WNDPROC)SetWindowLongPtr(cmdHwnd,GWLP_WNDPROC,(LONG_PTR)CmdBoxProc);
			SetWindowLong(cmdHwnd,GWLP_USERDATA,(LONG_PTR)cview);

			mainModel->logHwnd = logHwnd;
			break;
		}

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(cview->logHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top-24,TRUE);
			MoveWindow(cview->cmdHwnd,0,rect.bottom-rect.top-24,rect.right-rect.left,24,TRUE);
			break;
		}

		case WM_LBUTTONDOWN:{
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK CmdBoxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	ConsoleView * consoleView = (ConsoleView *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	char buff[256];

	switch(msg){
		case WM_KEYUP:
			switch(wParam){
				case VK_UP:
					//model->cl->rollup();
					//view->UpdateData(*model);
				break;

				case VK_DOWN:
					//model->cl->rolldown();
					//view->UpdateData(*model);
				break;

				case VK_RETURN:
					GetWindowText(hwnd,buff,256);

					//char * cmds = parse_command((char *)buff,256);
					//char * comment = build_comment((char *)buff,cmds);
					int curLeng = GetWindowTextLength(consoleView->logHwnd);
					SendMessage(consoleView->logHwnd,EM_SETSEL,curLeng,curLeng);
					//SendMessage(consoleView->logHwnd,EM_REPLACESEL,FALSE,(LPARAM)comment);

                    //connection_senddata(mainModel->connection,cmds,strlen(cmds),0);
					
					SetWindowText(hwnd,NULL);
					
					//free(cmds);
					//free(comment);
				break;
			}
		break;
	}

	return CallWindowProc(consoleView->defaultCmdProc,hwnd,msg,wParam,lParam);
}