#include "dataview.h"

void initCombox(HWND viewTypeHwnd){
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_USEREX_CLASSES;
    InitCommonControlsEx(&icex);

    COMBOBOXEXITEM cbei;
    int iCnt;
    
    typedef struct {
        int iImage;
        int iSelectedImage;
        int iIndent;
        LPTSTR pszText;
    } ITEMINFO, *PITEMINFO;

    ITEMINFO IInf[] = {
        { 0, 0,  0, "Json"}, 
        { 0, 0,  0, "Binary"},
        { 0, 0,  0, "Text"}
    };

    // Set the mask common to all items.
    cbei.mask = CBEIF_TEXT | CBEIF_INDENT |
                CBEIF_IMAGE| CBEIF_SELECTEDIMAGE;

    for(iCnt=0; iCnt<3; iCnt++){
        cbei.iItem          = iCnt;
        cbei.pszText        = IInf[iCnt].pszText;
        cbei.cchTextMax     = sizeof(IInf[iCnt].pszText);
        cbei.iImage         = IInf[iCnt].iImage;
        cbei.iSelectedImage = IInf[iCnt].iSelectedImage;
        cbei.iIndent        = IInf[iCnt].iIndent;
        
		SendMessage(viewTypeHwnd,CBEM_INSERTITEM,0,(LPARAM)&cbei);
    }

	HIMAGELIST hImageList=ImageList_Create(14,14,ILC_COLOR|ILC_MASK,2,10);
	HBITMAP hBitmap = LoadBitmap(mainModel->hInstance,MAKEINTRESOURCE(IDB_CHIP)); //


	ImageList_Add(hImageList,hBitmap,NULL);

    SendMessage(viewTypeHwnd,CBEM_SETIMAGELIST,0,(LPARAM)hImageList);
} 

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	DataView * dataView = mainModel->dataView;
	RECT rect;

    switch(msg){
		case WM_CREATE:{
			LONG_PTR hinst = mainModel->hInstance;

            HWND keyNameHwnd   = CreateWindowEx(0, WC_STATIC, (""), WS_VISIBLE | WS_BORDER | WS_CHILD | WS_GROUP | SS_LEFT, 5, 5, 40, 24, dataHwnd, (HMENU)0, hinst, 0);
            HWND keyEditHwnd   = CreateWindowEx(0, WC_EDIT,   (""), WEDIS_EDIT_STYLE, 50, 5, 180, 24, dataHwnd, (HMENU)GENERAL_CMD_KEYEDIT, hinst, 0);    
            HWND renameBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Rename"), WEDIS_PUSH_BUTTON_STYLE, 250, 5, 60, 24, dataHwnd, GENERAL_CMD_RENAME, hinst, 0);     
            HWND ttlBtnHwnd    = CreateWindowEx(0, WC_BUTTON, ("TTL"), WEDIS_PUSH_BUTTON_STYLE, 315, 5, 60, 24, dataHwnd, GENERAL_CMD_SETTTL, hinst, 0);
            HWND removeBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Remove"), WEDIS_PUSH_BUTTON_STYLE, 380, 5, 60, 24, dataHwnd, GENERAL_CMD_REMOVE, hinst, 0);
            HWND reloadBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Reload"), WEDIS_PUSH_BUTTON_STYLE, 445, 5, 60, 24, dataHwnd, GENERAL_CMD_RELOAD, hinst, 0);
            HWND viewTypeHwnd  = CreateWindowEx(0, WC_COMBOBOXEX, (""), WEDIS_COMBO_BOX_STYLE,510, 5, 120, 100, dataHwnd, (HMENU)0, hinst, 0);

			initCombox(viewTypeHwnd);
            
            HWND dataViewHwnd  = CreateWindowEx(0, DATA_RENDER_WINDOW, (""), 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                5, 34, 625, 240, 
                dataHwnd, (HMENU)0, hinst, 0);

			dataView->hashViewHwnd = buildHashViewWindow(dataViewHwnd);
			dataView->stringViewHwnd = buildStringViewWindow(dataViewHwnd);
			dataView->listViewHwnd = buildListViewWindow(dataViewHwnd);
			dataView->setViewHwnd = buildSetViewWindow(dataViewHwnd);
			dataView->zsetViewHwnd = buildZsetViewWindow(dataViewHwnd);

            HWND saveBtnHwnd  = CreateWindowEx(0, WC_BUTTON,  ("Save"), WEDIS_PUSH_BUTTON_STYLE, 570, 279, 60, 24, dataHwnd, (HMENU)0, hinst, 0);     
            HWND exportBtnHwnd = CreateWindowEx(0, WC_BUTTON, ("Export"), WEDIS_PUSH_BUTTON_STYLE, 505, 279, 60, 24, dataHwnd, (HMENU)0, hinst, 0); 
            
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
			SendMessage(keyNameHwnd, WM_SETFONT, (WPARAM)hfont0, FALSE);

			dataView->dataViewHwnd  = dataViewHwnd;
			dataView->exportBtnHwnd = exportBtnHwnd;
			dataView->saveBtnHwnd   = saveBtnHwnd;
			dataView->viewTypeHwnd  = viewTypeHwnd;

			dataView->reloadBtnHwnd = reloadBtnHwnd;
			dataView->removeBtnHwnd = removeBtnHwnd;
			dataView->renameBtnHwnd = renameBtnHwnd;
			dataView->ttlBtnHwnd    = ttlBtnHwnd;
			dataView->keyEditHwnd   = keyEditHwnd;
			dataView->keyNameHwnd   = keyNameHwnd;

			SetWindowLong(dataHwnd,GWLP_USERDATA,(LONG)dataView);
		    break;
		}

        case WM_COMMAND:{
			switch(LOWORD (wParam)){
				case GENERAL_CMD_RENAME:{
                    TCHAR newKey[256]={0};
                    GetDlgItemText(dataHwnd,GENERAL_CMD_KEYEDIT,newKey,sizeof(newKey));
					
					redis_rename_key(dataView->data->dataKey,newKey);
					break;
				}
				
				case GENERAL_CMD_SETTTL:{
					DialogBox (mainModel->hInstance,MAKEINTRESOURCE (IDD_GOTOLINE),dataHwnd,(DLGPROC)SetTtlDlgProc);
					break;
				}

				case GENERAL_CMD_REMOVE:{
					redis_delete_key(dataView->data->dataKey);
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
			RedisReply * data = (RedisReply *)wParam;

			SendMessage(dataView->keyEditHwnd,WM_SETTEXT,0,(LPARAM)(data->dataKey));
			SendMessage(dataView->keyNameHwnd,WM_SETTEXT,0,(LPARAM)(data->dataTypeName));

            dataView->data = data;
            dataView->type =  data->dataType;

            // 打开数据按钮状态

			switchView(dataHwnd,data->dataType,data);
			break;
		}

		case WM_SIZE:{
			GetClientRect(dataHwnd,&rect);

			MoveWindow(dataView->dataViewHwnd,5,34,rect.right-rect.left-5-5,rect.bottom-rect.top-68-5,TRUE);
			MoveWindow(dataView->exportBtnHwnd,rect.right-rect.left-60-5,rect.bottom-rect.top-24-5,60,24,TRUE);
			MoveWindow(dataView->saveBtnHwnd,rect.right-rect.left-125-5,rect.bottom-rect.top-24-5,60,24,TRUE);

			MoveWindow(dataView->viewTypeHwnd,rect.right-rect.left-120-5,5,120,100,TRUE);
			MoveWindow(dataView->reloadBtnHwnd,rect.right-rect.left-5-120-5-60,5,60,24,TRUE);
			MoveWindow(dataView->removeBtnHwnd,rect.right-rect.left-5-120-5-60-5-60,5,60,24,TRUE);
			MoveWindow(dataView->ttlBtnHwnd,rect.right-rect.left-5-120-5-60-5-60-5-60,5,60,24,TRUE);
			MoveWindow(dataView->renameBtnHwnd,rect.right-rect.left-5-120-5-60-5-60-5-60-5-60,5,60,24,TRUE);
			MoveWindow(dataView->keyEditHwnd,50,5,rect.right-rect.left-40-5-120-5-60-5-60-5-60-5-60-5-5-5,24,TRUE);

			break;
		}
	}

	return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

void switchView(HWND hwnd,int type,RedisReply * reply){
	DataView * dataView = mainModel->dataView;

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