#include "dataview.h"

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	DataView * dataView = App->dataView;
	RECT rect;

    switch(msg){
		case WM_CREATE:{
			HINSTANCE hinst = App->hInstance;
			
			dataView->hashViewHwnd   = CreateWindowEx(0, HASH_VIEW_CLASS,NULL, (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);
			dataView->stringViewHwnd = CreateWindowEx(0, STRING_VIEW_CLASS,NULL, (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);
			dataView->listViewHwnd   = CreateWindowEx(0, LIST_VIEW_CLASS,NULL, (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0,0,0,0,dataHwnd, (HMENU)0, App->hInstance,0);
			dataView->setViewHwnd    = CreateWindowEx(0, SET_VIEW_CLASS,NULL, (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);;
			dataView->zsetViewHwnd   = CreateWindowEx(0, ZSET_VIEW_CLASS,NULL, (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);
			dataView->systemViewHwnd = CreateWindowEx(0, SYSTEM_VIEW_CLASS,NULL,(!WS_VISIBLE)| WS_CHILD,0,0,0,0,dataHwnd,0,hinst,NULL);

			ShowWindow(dataView->systemViewHwnd,SW_HIDE);
			SetWindowLongPtr(dataHwnd,GWLP_USERDATA,(LONG_PTR)dataView);
		    break;
		}

		case WM_NOTIFY:{
			LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
				case LVN_DELETEALLITEMS:{
					MessageBox(dataHwnd,"data removed.","hello",MB_OK);
					break;
				}
			}

			MessageBox(dataHwnd,"data removed.","hello",MB_OK);
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
					DialogBox (App->hInstance,MAKEINTRESOURCE (IDD_GOTOLINE),dataHwnd,(DLGPROC)SetTtlDlgProc);
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
			
			int dataType = REDIS_SYSTEM;
			if(datanode->level != NODE_LEVEL_HOST){
				//SendMessage(dataView->systemViewHwnd,WM_DT,(WPARAM)data,(LPARAM)(datanode));
				// switchView(dataHwnd,datanode->data->data_type,data);
			//}else{
				dataType = datanode->data->data_type;
			}

			switchView(dataHwnd,dataType,data);

			break;
		}

		case WM_SIZE:{
			GetClientRect(dataHwnd,&rect);
			MoveWindow(dataView->hashViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->stringViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->listViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->setViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->zsetViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			MoveWindow(dataView->systemViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
			break;
		}
	}

	return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

void switchView(HWND hwnd,int type,RedisReply data){
	DataView * dataView = App->dataView;
	HWND lastVisible = dataView->visibleHwnd;
	
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

		case REDIS_SYSTEM:{
			dataView->visibleHwnd = dataView->systemViewHwnd;
			break;
		}
	}

	SendMessage(dataView->visibleHwnd,WM_DT,(WPARAM)data,(LPARAM)NULL);
	ShowWindow(dataView->visibleHwnd,SW_SHOW);
	if(lastVisible != dataView->visibleHwnd){
		ShowWindow(lastVisible,SW_HIDE);
	}
}
