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
    
    int total = appConfig->total_host;
	for(int ix =0 ;ix < total ; ix ++){
        Host * host = appConfig->hosts[ix];
		host->hostIndex = IDM_CUSTOMER_HOST + ix;
    
        tvinsert.hParent             = model->parentTree;
	    tvinsert.hInsertAfter        = TVI_ROOT;
	    tvinsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	    tvinsert.item.iImage         = 2;
	    tvinsert.item.iSelectedImage = 2;
        tvinsert.item.pszText        = host->host;
        tvinsert.item.lParam         = (LPARAM)host;
		
		SendMessage(model->hostTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	}
}

BOOL CALLBACK conectionConfigDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam){
	char buff[MAX_PATH];
    HostModel model = (HostModel)GetWindowLongPtr(hWin,GWLP_USERDATA);

	switch(msg){
		case WM_INITDIALOG:{
			//HINSTANCE hInstance = mainModel->hInstance;
			//LoadString(hInstance,IDS_CODEVIEW,buff,MAX_PATH);
			//SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);

            model = (HostModel) calloc(1,sizeof(struct wedis_hosts_model));
            SetWindowLongPtr(hWin,GWLP_USERDATA,model);
            //buildConnectionTree(hWin);

			MoveToScreenCenter(hWin);

			buildConnectionTree(hWin);

            SendMessage(model->hostTree,TVM_EXPAND,(WPARAM)TVE_EXPAND,(LPARAM)model->parentTree);

			break;
		}

        case WM_NOTIFY:{
			LPNMHDR msg = ((LPNMHDR) lParam);
			switch (msg->code) {
				case NM_CLICK:{
                    DWORD dwPos = GetMessagePos();
                    POINT pt;
                    pt.x = LOWORD(dwPos);
                    pt.y = HIWORD(dwPos);
                    ScreenToClient(GetDlgItem(hWin,IDC_CONNECTION_TREE), &pt);

                    TVHITTESTINFO ht = {0};
                    ht.pt = pt;
                    ht.flags = TVHT_ONITEM;
                    HTREEITEM hItem = TreeView_HitTest(GetDlgItem(hWin,IDC_CONNECTION_TREE), &ht);

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
                        SetDlgItemText(hWin,9999,host->host);
                        SetDlgItemText(hWin,9997,host->password);
                        SetDlgItemText(hWin,9996,host->name);
                        SetDlgItemText(hWin,9995,host->description);

                        SetDlgItemInt(hWin,9998,host->port,FALSE);

                        Button_SetCheck(GetDlgItem(hWin,9994),host->requirepass);
                    }

					break;
				}
			}

			break;
		}

		case WM_COMMAND:{
			switch(wParam){
                case 9999:{
                    MessageBox(hWin,"sdsd","title",MB_OK);
                    break;
                }

				case IDOK:
                    save_config();
					EndDialog(hWin,0);
				break;

				case IDCANCEL:
					EndDialog(hWin,0);
				break;

                case 9994:{
                    UINT status =  IsDlgButtonChecked(hWin,9994);
                    char buff[255] = {0};
                    wsprintf(buff,"clicked,status: %d",status);
                    MessageBox(hWin,buff,"title",MB_OK);
                    break;
                }
			}
			break;
        }

		case WM_CLOSE:{
			EndDialog(hWin,0);
		    break;
        }
	}

	return FALSE;
}