#include "console.h"

/**
 * 命令控制窗口
 */
LRESULT CALLBACK consoleWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	ConsoleView * consoleView = (ConsoleView *)GetWindowLong(hwnd,GWL_USERDATA);

	RECT rect;

    switch(msg){
		case WM_CREATE:{
			consoleView = (ConsoleView*)malloc(sizeof(ConsoleView));
			memset(consoleView,0,sizeof(ConsoleView));
			
			HWND logHwnd  = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | WS_VSCROLL  |ES_MULTILINE, 0, 0, 100, 100, hwnd, (HMENU)0, hinst, 0);
            HWND cmdHwnd  = CreateWindowEx(0, WC_EDIT, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER , 0, 100, 100, 24, hwnd, (HMENU)0, hinst, 0);
            
			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(logHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(cmdHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);

			consoleView->logHwnd = logHwnd;
			consoleView->cmdHwnd = cmdHwnd;
			SetWindowLong(hwnd,GWL_USERDATA,(LONG)consoleView);

			consoleView->defaultCmdProc = (WNDPROC)SetWindowLong(cmdHwnd,GWL_WNDPROC,(LONG)CmdBoxProc);
			SetWindowLong(cmdHwnd,GWL_USERDATA,(LONG)consoleView);

			HWND hParent = GetParent(hwnd);
			MainModel * mainModel = (MainModel *)GetWindowLong(hParent,GWL_USERDATA);
			mainModel->logHwnd = logHwnd;

			consoleView->mainModel = mainModel;
			break;
		}

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
			MoveWindow(consoleView->logHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top-24,TRUE);
			MoveWindow(consoleView->cmdHwnd,0,rect.bottom-rect.top-24,rect.right-rect.left,24,TRUE);
			break;
		}

		case WM_LBUTTONDOWN:{
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK CmdBoxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	ConsoleView * cmd = (ConsoleView *) GetWindowLong(hwnd,GWL_USERDATA);
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
					//HWND parent = GetParent(hwnd);

					char * cmds = parse_command((char *)buff,256);
					char * comment = build_comment((char *)buff,cmds);
					int curLeng = GetWindowTextLength(cmd->logHwnd);
					SendMessage(cmd->logHwnd,EM_SETSEL,curLeng,curLeng);
					SendMessage(cmd->logHwnd,EM_REPLACESEL,FALSE,(LONG)comment);

					//cmd->mainModel->tcpClient->senddata(cmds,strlen(cmds),0);
                    cmd->mainModel->connection->cmdType = PT_DATA;
                    connection_senddata(cmd->mainModel->connection,cmds,strlen(cmds),0);
					//
					//Command * pcmd = (Command *) GetWindowLong(hwnd,GWL_USERDATA);
					//cmd->mainModel->tcpClient->senddata(cmds,strlen(cmds),0);
					//
					SetWindowText(hwnd,NULL);
					//
					free(cmds);
					free(comment);
				break;
			}
		break;
	}

	return CallWindowProc(cmd->defaultCmdProc,hwnd,msg,wParam,lParam);
}