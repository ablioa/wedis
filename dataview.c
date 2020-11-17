#include "dataview.h"

// void buildMyToolBar(HWND parent){
// 	HINSTANCE hInst = mainModel->hInstance;
// 	DWORD tstyle = WS_CHILD | CCS_VERT  | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;
// 	RECT  rect;

//     int buttonCount = 10;
	
// 	TBBUTTON tbtn[10] = {
//         {(0), IDM_CONNECTION, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
//         {(1), IDM_PREFERENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
// 		{(7), IDM_SYSTEM_STAT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
//         {(8), IDM_DEBUG_GET_DATABASES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
//         {(0), 0             , 0,               TBSTYLE_SEP,    {0}, 0, 0},
// 		{(2), IDM_ADD       , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
//         {(3), IDM_REMOVE    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
//         {(4), IDM_RELOAD    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
// 		{(5), IDM_RENAME    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
//         {(6), IDM_TIMING    , TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
//     };

// 	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_MAIN));
//     HIMAGELIST hIcons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, buttonCount);
// 	ImageList_AddMasked(hIcons, hBmp, RGB(255,255,255));

//     HWND tb = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle, 16, 16, 16, 16, parent, (HMENU) 0, hInst, NULL);
//     SendMessage(tb, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
//     SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) hIcons);
//     SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
//     SendMessage(tb, TB_ADDBUTTONS,       (WPARAM)buttonCount,       (LPARAM)&tbtn);
//     SendMessage(tb, TB_AUTOSIZE, 0, 0);

//     ShowWindow(tb,  TRUE);
// }

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	DataView * dataView = mainModel->dataView;

    switch(msg){
		case WM_CREATE:{
			createDataViewWindow(dataHwnd);
		    break;
		}

        case WM_COMMAND:{
			switch(LOWORD (wParam)){
				case GENERAL_CMD_RENAME:{
                    // TCHAR newKey[256]={0};
                    // GetDlgItemText(dataHwnd,GENERAL_CMD_KEYEDIT,newKey,sizeof(newKey));
				
					// redis_rename_key(dataView->data->dataKey,newKey);
					break;
				}
				
				case GENERAL_CMD_SETTTL:{
					DialogBox (mainModel->hInstance,MAKEINTRESOURCE (IDD_GOTOLINE),dataHwnd,(DLGPROC)SetTtlDlgProc);
					break;
				}

				case GENERAL_CMD_REMOVE:{
					// redis_delete_key(dataView->data->dataKey);
					break;
				}

				case GENERAL_CMD_RELOAD:{
					MessageBox(dataHwnd,"reload the data","title",MB_OK);
					break;
				}
			}
			break;
        }

		case WM_DT:{
			RedisReply data     = (RedisReply) wParam;
            TreeNode * datanode = (TreeNode *) lParam;

			// SendMessage(dataView->keyEditHwnd,WM_SETTEXT,0,(LPARAM)(data->dataKey));
			// SendMessage(dataView->keyNameHwnd,WM_SETTEXT,0,(LPARAM)(data->dataTypeName));

            // dataView->data = data;
            // dataView->type =  data->dataType;
			switchView(dataHwnd,datanode->data->data_type,data);

			break;
		}

		case WM_SIZE:{
			resizeDataViewWinwdow(dataHwnd);
			break;
		}
	}

	return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

void resizeDataViewWinwdow(HWND dataHwnd){
	DataView * dataView = mainModel->dataView;
	RECT rect;

	GetClientRect(dataHwnd,&rect);

    MoveWindow(dataView->dataViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
}

void createDataViewWindow(HWND dataHwnd){
    DataView * dataView = mainModel->dataView;
    HINSTANCE hinst = mainModel->hInstance;
    
    HWND dataViewHwnd  = CreateWindowEx(0, DATA_RENDER_WINDOW, (""), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
        0, 0, 625, 240, 
        dataHwnd, (HMENU)0, hinst, 0);
    
    dataView->hashViewHwnd = buildHashViewWindow(dataViewHwnd);
    dataView->stringViewHwnd = buildStringViewWindow(dataViewHwnd);
    dataView->listViewHwnd = buildListViewWindow(dataViewHwnd);
    dataView->setViewHwnd = buildSetViewWindow(dataViewHwnd);
    dataView->zsetViewHwnd = buildZsetViewWindow(dataViewHwnd);
    
    dataView->dataViewHwnd  = dataViewHwnd;

    // buildMyToolBar(dataViewHwnd);
    
    SetWindowLongPtr(dataHwnd,GWLP_USERDATA,(LONG_PTR)dataView);
}

void switchView(HWND hwnd,int type,RedisReply data){
	DataView * dataView = mainModel->dataView;

	ShowWindow(mainModel->view->dataHwnd,SW_SHOW);
	ShowWindow(dataView->visibleHwnd,SW_HIDE);
	ShowWindow(mainModel->view->systemViewHwnd,SW_HIDE);
	switch(type){
		case REDIS_STRING:{
			dataView->visibleHwnd = dataView->stringViewHwnd;
			break;
		}

		case REDIS_LIST:{
			dataView->visibleHwnd = dataView->listViewHwnd;
			break;
		}
		
		case REDIS_HASH:{
			dataView->visibleHwnd = dataView->hashViewHwnd;
			break;
		}

		case REDIS_SET:{
			dataView->visibleHwnd = dataView->setViewHwnd;
			break;
		}
		
		case REDIS_ZSET:{
			dataView->visibleHwnd = dataView->zsetViewHwnd;
			break;
		}
	}

	ShowWindow(dataView->visibleHwnd,SW_SHOW);
	SendMessage(dataView->visibleHwnd,WM_DT,(WPARAM)data,(LPARAM)NULL);
}

LRESULT CALLBACK dataRenderProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){

	DataView * dataView = mainModel->dataView;
	RECT rect;

	switch(message){
	    case WM_SIZE:{
			GetClientRect(hwnd,&rect);

			MoveWindow(dataView->hashViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->stringViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->listViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->setViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->zsetViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			break;
	    }
	}

    return DefWindowProc (hwnd, message, wParam, lParam);
}
