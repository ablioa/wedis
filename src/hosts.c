#include "hosts.h"

#include <windowsx.h>

void buildConnectionTree(HWND hwnd){
    HostModel model = (HostModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);
    RECT            rt;
    GetWindowRect(hwnd,&rt);

    model->hostTree = GetDlgItem(hwnd,IDC_CONNECTION_TREE);

    SendMessage(model->hostTree,TVM_SETIMAGELIST,0,(LPARAM)resource->icons);

    TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

    tvinsert.hParent = NULL;
    tvinsert.hInsertAfter=TVI_ROOT;
    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
    tvinsert.item.iImage         = TREE_FOLDER_NODE;
    tvinsert.item.iSelectedImage = TREE_FOLDER_NODE;

    char * buff = (char *)calloc(1,255);
    LoadString(App->hInstance,IDS_HOSTLIST,buff,255);
    tvinsert.item.pszText= buff;

    model->parentTree = (HTREEITEM)SendMessage(model->hostTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);


    Host * start = appConfig->head;
    while(start != NULL){
        Host * host = start;

        tvinsert.hParent             = model->parentTree;
        tvinsert.hInsertAfter        = TVI_ROOT;
        tvinsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
        tvinsert.item.iImage         = 2;
        tvinsert.item.iSelectedImage = 2;
        tvinsert.item.pszText        = host->name;
        tvinsert.item.lParam         = (LPARAM)host;

        HTREEITEM item = (HTREEITEM)SendMessage(model->hostTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
        SendMessage(model->hostTree,TVM_SELECTITEM,(WPARAM)TVGN_CARET,(LPARAM) item);

        start = start->next;
    }
}


LRESULT CALLBACK nameEditProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
    HWND hParent        = GetParent(hwnd);
    HostModel model     = (HostModel)GetWindowLongPtr(hParent,GWLP_USERDATA);

    switch (message){
        case WM_KEYUP:{
            char buff[MAX_PATH] = {0};
            Host * host = get_host_by_index(appConfig,model->hostIndex);
            GetDlgItemText(hParent,IDC_CONNECTION_VALUE,buff,MAX_PATH);
            memcpy(host->name,buff,MAX_PATH);
            break;
        }

        default:{
            break;
        }
    }

    return CallWindowProc(model->nameEditProc,hwnd,message,wParam,lParam);
}

LRESULT CALLBACK hostEditProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
    HWND hParent = GetParent(hwnd);
    HostModel model = (HostModel)GetWindowLongPtr(hParent,GWLP_USERDATA);

    switch (message){
        case WM_KEYUP:{
            char buff[MAX_PATH] = {0};
            Host * host = get_host_by_index(appConfig,model->hostIndex);
            GetDlgItemText(hParent,IDC_CONNECTION_HOSTVALUE,buff,MAX_PATH);
            memcpy(host->host,buff,MAX_PATH);
            break;
        }

        default:{
            break;
        }
    }

    return CallWindowProc(model->hostEditProc,hwnd,message,wParam,lParam);
}

LRESULT CALLBACK portEditProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
    HWND hParent = GetParent(hwnd);
    HostModel model = (HostModel)GetWindowLongPtr(hParent,GWLP_USERDATA);
    BOOL result = TRUE;
    switch (message){
        case WM_KEYUP:{
            Host * host = get_host_by_index(appConfig,model->hostIndex);
            UINT port   = GetDlgItemInt(hParent,IDC_CONNECTION_PORTVALUE,&result,FALSE);
            if(result){
                host->port = port;
            }

            break;
        }

        default:{
            break;
        }
    }

    return CallWindowProc(model->portEditProc,hwnd,message,wParam,lParam);
}

LRESULT CALLBACK passwordEditProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
    HWND hParent = GetParent(hwnd);
    HostModel model = (HostModel)GetWindowLongPtr(hParent,GWLP_USERDATA);

    switch (message){
        case WM_KEYUP:{
            Host * host = get_host_by_index(appConfig,model->hostIndex);

            char buff[MAX_PATH] = {0};
            GetDlgItemText(hParent,IDC_CONNECTION_PASSWORDVALUE,buff,MAX_PATH);
            memcpy(host->password,buff,MAX_PATH);
            break;
        }

        default:{
            break;
        }
    }

    return CallWindowProc(model->passwordEditProc,hwnd,message,wParam,lParam);
}

LRESULT CALLBACK descEditProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
    HWND hParent = GetParent(hwnd);
    HostModel model = (HostModel)GetWindowLongPtr(hParent,GWLP_USERDATA);

    switch (message){
        case WM_KEYUP:{
            Host * host = get_host_by_index(appConfig,model->hostIndex);

            char buff[MAX_PATH] = {0};
            GetDlgItemText(hParent,IDC_CONNECTION_DESCRIPTION,buff,MAX_PATH);
            memcpy(host->description,buff,MAX_PATH);

            break;
        }

        default:{
            break;
        }
    }

    return CallWindowProc(model->descEditProc,hwnd,message,wParam,lParam);
}

void updateConfigure(HWND hwnd,Host * host){
    HostModel model = (HostModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);
    model->hostIndex = host->hostIndex;

    SetDlgItemText(hwnd,IDC_CONNECTION_HOSTVALUE,host->host);
    SetDlgItemText(hwnd,IDC_CONNECTION_PASSWORDVALUE,host->password);
    SetDlgItemText(hwnd,IDC_CONNECTION_VALUE,host->name);
    SetDlgItemText(hwnd,IDC_CONNECTION_DESCRIPTION,host->description);

    SetDlgItemInt(hwnd,IDC_CONNECTION_PORTVALUE,host->port,FALSE);

    EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_HOSTVALUE), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_VALUE), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_PORTVALUE), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_IFPASSWORD), TRUE);
    EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_DESCRIPTION), TRUE);

    Button_SetCheck(GetDlgItem(hwnd,IDC_CONNECTION_IFPASSWORD),host->requirepass);
    if (!host->requirepass){
        EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_PASSWORDVALUE), FALSE);
    }
    else{
        EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_PASSWORDVALUE), TRUE);
    }
}

BOOL CALLBACK conectionConfigDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    HostModel model = (HostModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch(msg){
        case WM_INITDIALOG:{
            model = (HostModel) calloc(1,sizeof(struct wedis_hosts_model));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

            model->hostEditProc     = (WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_HOSTVALUE),GWLP_WNDPROC,(LONG_PTR)hostEditProc);
            model->nameEditProc     = (WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_VALUE),GWLP_WNDPROC,(LONG_PTR)nameEditProc);
            model->portEditProc     = (WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_PORTVALUE),GWLP_WNDPROC,(LONG_PTR)portEditProc);
            model->passwordEditProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_PASSWORDVALUE),GWLP_WNDPROC,(LONG_PTR)passwordEditProc);
            model->descEditProc     = (WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_DESCRIPTION),GWLP_WNDPROC,(LONG_PTR)descEditProc);

            MoveToScreenCenter(hwnd);
            buildConnectionTree(hwnd);

            EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_HOSTVALUE), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_VALUE), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_PORTVALUE), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_PASSWORDVALUE), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_IFPASSWORD), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_DESCRIPTION), FALSE);
            break;
        }

        case WM_NOTIFY:{
            LPNMHDR msg = ((LPNMHDR) lParam);
            switch (msg->code) {
                case TVN_SELCHANGED:{
                    LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW) lParam;
                     TVITEM item;
                     item.mask = TVIF_PARAM;
                     item.hItem = pNMTreeView->itemNew.hItem;

                     TreeView_GetItem(model->hostTree,&item);
                     Host * host = (Host*)item.lParam;

                    if(host == NULL){
                        break;
                    }

                    model->selectedTree = item.hItem;

                    updateConfigure(hwnd,host);
                    break;
                }
                case NM_CLICK:{
                    DWORD dwPos = GetMessagePos();
                    POINT pt;
                    pt.x = LOWORD(dwPos);
                    pt.y = HIWORD(dwPos);
                    ScreenToClient(GetDlgItem(hwnd,IDC_CONNECTION_TREE), &pt);

                    TVHITTESTINFO ht = {0};
                    ht.pt = pt;
                    ht.flags = TVHT_ONITEM;
                    HTREEITEM hItem = TreeView_HitTest(GetDlgItem(hwnd,IDC_CONNECTION_TREE), &ht);

                    if(hItem == NULL){
                        break;
                    }

                    TVITEM ti = {0};
                    ti.mask = TVIF_HANDLE | TVIF_PARAM;
                    ti.hItem = hItem;
                    TreeView_GetItem(App->view->overviewHwnd, &ti);

                    Host * host = (Host *) ti.lParam;
                    if(host != NULL){
                        updateConfigure(hwnd,host);
                    }

                    break;
                }

                case NM_DBLCLK:{
                    HostModel model = (HostModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);
                    save_config();
                    EndDialog(hwnd,0);
                    SendMessage(GetParent(hwnd),WM_NEW_CONNECTION,(WPARAM)model->hostIndex,(LPARAM)NULL);
                    break;
                }
            }

            break;
        }

        case WM_COMMAND:{
            switch(wParam){
                case IDC_CONNECTION_ADD:{
                    TV_INSERTSTRUCT tvinsert;
                    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

                    tvinsert.hParent = model->parentTree;
                    tvinsert.hInsertAfter=TVI_ROOT;
                    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
                    tvinsert.item.iImage=2;
                    tvinsert.item.iSelectedImage=2;

                    Host * host       = (Host *) calloc(1,sizeof(Host));
                    host->description = (char*)calloc(256,sizeof(char));
                    host->host        = (char*)calloc(256,sizeof(char));
                    host->password    = (char*)calloc(256,sizeof(char));
                    host->name        = (char*)calloc(256,sizeof(char));

                    wsprintf(host->name,"%s","New Connection");

                    host->requirepass = 1;
                    host->port=1990;

                    add_host(host);

                    tvinsert.item.pszText  = host->name;
                    tvinsert.item.lParam   = (LPARAM)host;

                    HTREEITEM item = (HTREEITEM)SendMessage(model->hostTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
                    SendMessage(model->hostTree,TVM_SELECTITEM,(WPARAM)TVGN_CARET,(LPARAM) item);
                    model->selectedTree = item;
                    break;
                }

                case IDC_CONNECTION_DELETE:{
                    SendMessage(model->hostTree, TVM_DELETEITEM,0,(LPARAM)model->selectedTree);
                    remove_host_config(model->hostIndex);
                    break;
                }

                case IDOK:{
                    save_config();
                    EndDialog(hwnd,0);
                    SendMessage(GetParent(hwnd),WM_UPDATE_CONNECTION_LIST,(WPARAM)NULL,(LPARAM)NULL);
                    break;
                }

                case IDCANCEL:
                    EndDialog(hwnd,0);
                break;

                case IDC_CONNECTION_IFPASSWORD:{
                    UINT status =  IsDlgButtonChecked(hwnd,IDC_CONNECTION_IFPASSWORD);
                    Host * host = get_host_by_index(appConfig,model->hostIndex);
                    if(status){
                        EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_PASSWORDVALUE), TRUE);
                        host->requirepass = 1;
                    }else{
                        EnableWindow(GetDlgItem(hwnd, IDC_CONNECTION_PASSWORDVALUE), FALSE);
                        host->requirepass = 0;
                    }
                    break;
                }

                case IDC_CONFIG_CONNECT:{
                    save_config();
                    EndDialog(hwnd,0);
                    SendMessage(GetParent(hwnd),WM_NEW_CONNECTION,(WPARAM)model->hostIndex,(LPARAM)NULL);
                    break;
                }
            }
            break;
        }

        case WM_CLOSE:{
            EndDialog(hwnd,0);
            break;
        }
    }

    return FALSE;
}
