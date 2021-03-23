#include "dataview.h"

LRESULT CALLBACK dataViewProc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    DataView * dataView = App->dataView;
    RECT rect;

    switch(msg){
        case WM_CREATE:{
            HINSTANCE hinst = App->hInstance;
                dataView->hashViewHwnd   = CreateWindowEx(0, HASH_VIEW_CLASS,NULL, 
                        (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                        0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);

                dataView->stringViewHwnd = CreateWindowEx(0, STRING_VIEW_CLASS,NULL, 
                        (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                        0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);

                dataView->listViewHwnd   = CreateWindowEx(0, LIST_VIEW_CLASS,NULL, 
                        (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                        0,0,0,0,dataHwnd, (HMENU)0, App->hInstance,0);

                dataView->setViewHwnd    = CreateWindowEx(0, SET_VIEW_CLASS,NULL, 
                        (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                        0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);

                dataView->zsetViewHwnd   = CreateWindowEx(0, ZSET_VIEW_CLASS,NULL, 
                        (!WS_VISIBLE) | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 
                        0,0,0,0,dataHwnd, (HMENU)0, hinst, 0);

                dataView->systemViewHwnd = CreateWindowEx(0, SYSTEM_VIEW_CLASS,NULL,
                        (!WS_VISIBLE)| WS_CHILD,
                        0,0,0,0,dataHwnd,0,hinst,NULL);

                dataView->databaseViewHwnd = CreateWindowEx(0, DATABASE_VIEW_CLASS,NULL,
                        (!WS_VISIBLE)| WS_CHILD,
                        0,0,0,0,dataHwnd,0,hinst,NULL);

                ShowWindow(dataView->systemViewHwnd,SW_HIDE);
                SetWindowLongPtr(dataHwnd,GWLP_USERDATA,(LONG_PTR)dataView);
            break;
        }

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
            switch (msg->code) {
                case LVN_DELETEALLITEMS:{
                    break;
                }
            }

            break;
        }

        case WM_COMMAND:{
            switch(LOWORD (wParam)){
                case GENERAL_CMD_RENAME:{
                    break;
                }
                
                case GENERAL_CMD_SETTTL:{
                    break;
                }

                case GENERAL_CMD_REMOVE:{
                    break;
                }

                case GENERAL_CMD_RELOAD:{
                    break;
                }
            }
            break;
        }

        case WM_DTT:{
            SendMessage(dataView->visibleHwnd,WM_DTT,wParam,lParam);
            break;
        }

        case WM_DT:{
            RedisReply data     = (RedisReply) wParam;
            TreeNode * datanode = (TreeNode *) lParam;

            int dataType = REDIS_SYSTEM;
            if(datanode->level == NODE_LEVEL_HOST){
                dataType = REDIS_SYSTEM;
            }else if(datanode->level == NODE_LEVEL_DATABASE){
                dataType = REDIS_DATABASE;
            }else{
                dataType = datanode->data->data_type;
            }

            switchView(dataHwnd,dataType,datanode,data);

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
            MoveWindow(dataView->databaseViewHwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            break;
        }
    }

    return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

void switchView(HWND hwnd,int type,TreeNode * treeNode,RedisReply data){
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

        case REDIS_DATABASE:{
            dataView->visibleHwnd = dataView->databaseViewHwnd;
            break;
        }

        case REDIS_SYSTEM:{
            dataView->visibleHwnd = dataView->systemViewHwnd;
            break;
        }
    }

    SendMessage(dataView->visibleHwnd,WM_DT,(WPARAM)data,(LPARAM)treeNode);
    ShowWindow(dataView->visibleHwnd,SW_SHOW);
    if(lastVisible != dataView->visibleHwnd){
        ShowWindow(lastVisible,SW_HIDE);
    }
}
