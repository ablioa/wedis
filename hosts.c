#include "hosts.h"

#include <windowsx.h>

void buildConnectionTree(HWND hwnd){
    HostModel model = (HostModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);
	RECT            rt;

	HINSTANCE hinst = mainModel->hInstance;
	GetWindowRect(hwnd,&rt);

    model->hostTree = GetDlgItem(hwnd,IDC_CONNECTION_TREE);

	HIMAGELIST hImageList=ImageList_Create(16,16,ILC_COLOR24|ILC_MASK,2,10);
	HBITMAP hBitmap = LoadBitmap(hinst,MAKEINTRESOURCE(IDB_CONNECTION));
	ImageList_AddMasked(hImageList, hBitmap, RGB(255,255,255));
	SendMessage(model->hostTree,TVM_SETIMAGELIST,0,(LPARAM)hImageList);

	TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

    tvinsert.hParent = NULL;
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	tvinsert.item.iImage=0;
	tvinsert.item.iSelectedImage=1;
    tvinsert.item.pszText= "product and internet service";

	//TreeNode * treeNode = buildTreeNode();
	//treeNode->level = 1;
	//tvinsert.item.lParam=(LPARAM)treeNode;

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
	HWND hParent = GetParent(hwnd);
	HostModel model = (HostModel)GetWindowLongPtr(hParent,GWLP_USERDATA);

	switch (message){
		case WM_KEYUP:{
			Host * host = getHostByIndex(appConfig,model->hostIndex);
			MessageBox(hwnd,host->name,"title",MB_OK);
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
			Host * host = getHostByIndex(appConfig,model->hostIndex);
			MessageBox(hwnd,host->host,"title",MB_OK);
			//GetDlgItemText(hParent,IDC_CONNECTION_HOSTVALUE);
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

	switch (message){
		case WM_KEYUP:{
			Host * host = getHostByIndex(appConfig,model->hostIndex);

			char msg[255] = {0};
			wsprintf(msg,"prot: %d",host->port);
			MessageBox(hwnd,msg,"title",MB_OK);
			//GetDlgItemText(hParent,IDC_CONNECTION_HOSTVALUE);
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
			Host * host = getHostByIndex(appConfig,model->hostIndex);

			char msg[255] = {0};
			wsprintf(msg,"prot: %d",host->port);
			MessageBox(hwnd,host->password,"title",MB_OK);
			//GetDlgItemText(hParent,IDC_CONNECTION_HOSTVALUE);
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
			Host * host = getHostByIndex(appConfig,model->hostIndex);

			char msg[255] = {0};
			wsprintf(msg,"prot: %d",host->port);
			MessageBox(hwnd,host->description,"title",MB_OK);
			//GetDlgItemText(hParent,IDC_CONNECTION_HOSTVALUE);
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
    
    Button_SetCheck(GetDlgItem(hwnd,IDC_CONNECTION_IFPASSWORD),host->requirepass);
	if(!host->requirepass){
		EnableWindow(GetDlgItem(hwnd,IDC_CONNECTION_PASSWORDVALUE),FALSE);
	}else{
		EnableWindow(GetDlgItem(hwnd,IDC_CONNECTION_PASSWORDVALUE),TRUE);
	}
}

BOOL CALLBACK conectionConfigDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	char buff[MAX_PATH];
    HostModel model = (HostModel)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(msg){
		case WM_INITDIALOG:{
			//HINSTANCE hInstance = mainModel->hInstance;
			//LoadString(hInstance,IDS_CODEVIEW,buff,MAX_PATH);
			//SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);

			// SendMessage(GetDlgItem(hWin,11),);
			model = (HostModel) calloc(1,sizeof(struct wedis_hosts_model));
			SetWindowLongPtr(hwnd,GWLP_USERDATA,model);

			model->hostEditProc = SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_HOSTVALUE),GWLP_WNDPROC,hostEditProc);
			model->nameEditProc = SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_VALUE),GWLP_WNDPROC,nameEditProc);
			model->portEditProc = SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_PORTVALUE),GWLP_WNDPROC,portEditProc);
			model->passwordEditProc = SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_PASSWORDVALUE),GWLP_WNDPROC,passwordEditProc);
            model->descEditProc = SetWindowLongPtr(GetDlgItem(hwnd,IDC_CONNECTION_DESCRIPTION),GWLP_WNDPROC,descEditProc);

			MoveToScreenCenter(hwnd);
			buildConnectionTree(hwnd);
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
 					Host * host = (TreeNode*) item.lParam;

					// FIXME 这里为什么会取不到数据?
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

                    /** 未选中任何节点 */
                    if(hItem == NULL){
                    	return;
                    }

                    TVITEM ti = {0};
                    ti.mask = TVIF_HANDLE | TVIF_PARAM;
                    ti.hItem = hItem;
                    TreeView_GetItem(mainModel->view->connectionHwnd, &ti);

                    Host * host = (Host *) ti.lParam;
                    if(host != NULL){
						updateConfigure(hwnd,host);
                    }

					break;
				}
			}

			break;
		}

		case WM_COMMAND:{
			switch(wParam){
                case 9999:{
                    MessageBox(hwnd,"sdsd","title",MB_OK);
                    break;
                }

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

					add_co_host(host);

					tvinsert.item.pszText  = host->name;
					tvinsert.item.lParam   = (LPARAM)host;

					HTREEITEM item = (HTREEITEM)SendMessage(model->hostTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
					SendMessage(model->hostTree,TVM_SELECTITEM,(WPARAM)TVGN_CARET,(LPARAM) item);
					model->selectedTree = item;
					break;
				}

				case IDC_CONNECTION_DELETE:{
					
					// TreeView_DeleteItem(model->hostTree,model->selectedTree);

					SendMessage(model->hostTree, TVM_DELETEITEM,0,model->selectedTree);
					remove_host_config(model->hostIndex);
					break;
				}

				case IDC_CONNECTION_TEST:{
					MessageBox(hwnd,"test host","title",MB_OK);
					break;
				}

				case IDOK:{
					save_all_host_config();
					EndDialog(hwnd,0);
					break;
				}

				case IDCANCEL:
					EndDialog(hwnd,0);
				break;

                case 9994:{
                    UINT status =  IsDlgButtonChecked(hwnd,9994);
                    char buff[255] = {0};
                    wsprintf(buff,"clicked,status: %d",status);
                    MessageBox(hwnd,buff,"title",MB_OK);
                    break;
                }

				case IDC_CONFIG_CONNECT:{
					// TODO connect to new server
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