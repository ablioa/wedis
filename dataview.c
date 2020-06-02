#include "dataview.h"

BOOL CALLBACK exportBtnProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	DataView * dataView = mainModel->dataView;
	switch(msg){
		case WM_LBUTTONUP:{
			char * filename = mGetOpenFileName(hwnd);
			MessageBox(hwnd,filename,"File get",MB_OK);
			break;
		}
	}

	return CallWindowProc(dataView->exportBtnProc,hwnd,msg,wParam,lParam);
}

BOOL CALLBACK ttlBtnProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	DataView * dataView = mainModel->dataView;
	switch(msg){
		case WM_LBUTTONUP:{
			HINSTANCE hInst= (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
			DialogBox (hInst,MAKEINTRESOURCE (IDD_GOTOLINE),hwnd,SetTtlDlgProc);
			break;
		}
	}
	return CallWindowProc(dataView->ttlBtnProc,hwnd,msg,wParam,lParam);;
}

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	DataView * dataView = mainModel->dataView;
	RECT rect;

    switch(msg){
		case WM_CREATE:{
			HINSTANCE hinst = (HINSTANCE)GetWindowLong(dataHwnd,GWL_HINSTANCE);

            HWND keyEditHwnd   = CreateWindowEx(0, WC_EDIT, ("11"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 5, 5, 240, 24, dataHwnd, (HMENU)0, hinst, 0);    
            HWND renameBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Rename"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 250, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);     
            HWND ttlBtnHwnd    = CreateWindowEx(0, WC_BUTTON, ("TTL"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 315, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);
            HWND removeBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Remove"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 380, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);
            HWND reloadBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Reload"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 445, 5, 60, 24, dataHwnd, (HMENU)0, hinst, 0);
            HWND viewTypeHwnd  = CreateWindowEx(0, WC_COMBOBOX, (""), CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,510, 5, 120, 24, dataHwnd, (HMENU)0, hinst, 0);
            
            HWND dataViewHwnd  = CreateWindowEx(0, DATA_RENDER_WINDOW, (""), 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 
                5, 34, 625, 240, 
                dataHwnd, (HMENU)0, hinst, 0);

			dataView->hashViewHwnd = buildHashViewWindow(dataViewHwnd);
			dataView->stringViewHwnd = buildStringViewWindow(dataViewHwnd);
			dataView->listViewHwnd = buildListViewWindow(dataViewHwnd);
			dataView->setViewHwnd = buildSetViewWindow(dataViewHwnd);
			dataView->zsetViewHwnd = buildZsetViewWindow(dataViewHwnd);

            HWND saveBtnHwnd  = CreateWindowEx(0, WC_BUTTON, ("Save"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, 570, 279, 60, 24, dataHwnd, (HMENU)0, hinst, 0);     
            HWND exportBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Export"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 505, 279, 60, 24, dataHwnd, (HMENU)0, hinst, 0); 
            
            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(dataViewHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(saveBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(exportBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(keyEditHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(renameBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(viewTypeHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(ttlBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(removeBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(reloadBtnHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);

			dataView->dataViewHwnd  = dataViewHwnd;
			dataView->exportBtnHwnd = exportBtnHwnd;
			dataView->saveBtnHwnd   = saveBtnHwnd;
			dataView->viewTypeHwnd  = viewTypeHwnd;

			dataView->reloadBtnHwnd = reloadBtnHwnd;
			dataView->removeBtnHwnd = removeBtnHwnd;
			dataView->renameBtnHwnd = renameBtnHwnd;
			dataView->ttlBtnHwnd    = ttlBtnHwnd;
			dataView->keyEditHwnd   = keyEditHwnd;

			SetWindowLong(dataHwnd,GWL_USERDATA,(LONG)dataView);

			dataView->ttlBtnProc = (WNDPROC)SetWindowLong(ttlBtnHwnd,GWL_WNDPROC,(LONG)ttlBtnProc);
			SetWindowLong(ttlBtnHwnd,GWL_USERDATA,(LONG)dataView);

			dataView->exportBtnProc = (WNDPROC)SetWindowLong(exportBtnHwnd,GWL_WNDPROC,(LONG)exportBtnProc);
			SetWindowLong(exportBtnHwnd,GWL_USERDATA,(LONG)dataView);

            //////////////////////////////////////////////////////////
            //TCHAR Planets[9][10] =  {
            //    TEXT("Mercury"),
            //    TEXT("Venus"), 
            //    TEXT("Terra"), 
            //    TEXT("Mars"), 
            //    TEXT("Jupiter"), 
            //    TEXT("Saturn"), 
            //    TEXT("Uranus"), 
            //    TEXT("Neptune"), 
            //    TEXT("Pluto") 
            //};
            //       
            //TCHAR A[16]; 
            //memset(&A,0,sizeof(A));       
            //for (int k = 0; k <= 8; k += 1){
            //    wcscpy_s(A, sizeof(A)/sizeof(TCHAR),  (TCHAR*)Planets[k]);
            //    SendMessage(viewTypeHwnd,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) A); 
            //}
            SendMessage(viewTypeHwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
            //////////////////////////////////////////////////////////
		    break;
		}

        case WM_LBUTTONDOWN:{
			break;
        }

		case WM_DT:{
			RedisReply * rp = (RedisReply *)wParam;
			int type = (int) lParam;

			MessageBox(dataHwnd,rp->key,"key",MB_OK);

			switchView(dataHwnd,type,rp);
			break;
		}

		case WM_SIZE:{
			GetClientRect(dataHwnd,&rect);

			MoveWindow(dataView->dataViewHwnd,5,34,rect.right-rect.left-5-5,rect.bottom-rect.top-68-5,TRUE);
			MoveWindow(dataView->exportBtnHwnd,rect.right-rect.left-60-5,rect.bottom-rect.top-24-5,60,24,TRUE);
			MoveWindow(dataView->saveBtnHwnd,rect.right-rect.left-125-5,rect.bottom-rect.top-24-5,60,24,TRUE);
			MoveWindow(dataView->viewTypeHwnd,rect.right-rect.left-120-5,5,120,24,TRUE);
			MoveWindow(dataView->reloadBtnHwnd,rect.right-rect.left-5-120-5-60,5,60,24,TRUE);
			MoveWindow(dataView->removeBtnHwnd,rect.right-rect.left-5-120-5-60-5-60,5,60,24,TRUE);
			MoveWindow(dataView->ttlBtnHwnd,rect.right-rect.left-5-120-5-60-5-60-5-60,5,60,24,TRUE);
			MoveWindow(dataView->renameBtnHwnd,rect.right-rect.left-5-120-5-60-5-60-5-60-5-60,5,60,24,TRUE);
			MoveWindow(dataView->keyEditHwnd,5,5,rect.right-rect.left-5-120-5-60-5-60-5-60-5-60-5-5,24,TRUE);

			break;
		}
	}

	return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

void switchView(HWND hwnd,int type,RedisReply * reply){
	DataView * dataView = mainModel->dataView;

	ShowWindow(dataView->visibleHwnd,SW_HIDE);
	switch(type){
		case 1:{
			dataView->visibleHwnd = dataView->stringViewHwnd;
			break;
		}
		case 2:{
			int size = reply->bulkSize;
			for(int ix = 0; ix < size; ix ++){
				char buff[256] = {0};
				wsprintf(buff,"size: %s %d -  %d",reply->bulks[ix],reply->bulkSize,type);
			}

			dataView->visibleHwnd = dataView->listViewHwnd;
			break;
		}
		case 3:{
			dataView->visibleHwnd = dataView->hashViewHwnd;
			break;
		}
		case 4:{
			dataView->visibleHwnd = dataView->setViewHwnd;
			break;
		}
		case 5:{
			dataView->visibleHwnd = dataView->zsetViewHwnd;
			break;
		}
	}

	ShowWindow(dataView->visibleHwnd,SW_SHOW);
	SendMessage(dataView->visibleHwnd,WM_DT,(WPARAM)reply,(LPARAM)NULL);
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