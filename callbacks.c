#include "callbacks.h"

void SetDialogIcon(HWND hWnd, int idi) {
   SendMessage(hWnd, WM_SETICON, ICON_BIG,  (LPARAM)LoadIcon((HINSTANCE) GetWindowLongPtr(hWnd, GWLP_HINSTANCE),MAKEINTRESOURCE(idi)));
   SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon((HINSTANCE) GetWindowLongPtr(hWnd, GWLP_HINSTANCE),MAKEINTRESOURCE(idi)));
}

void MoveToScreenCenter(HWND hwnd){
	RECT rt;
	int cx,cy;

	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(hwnd,&rt);
	MoveWindow(hwnd,(cx-rt.right+rt.left)>>1,(cy-rt.bottom+rt.top)>>1,rt.right-rt.left,rt.bottom-rt.top,TRUE);
}

BOOL CALLBACK ListItemEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
        case WM_INITDIALOG:{
			SetDialogIcon(hwnd,IDI_MAIN);
            MoveToScreenCenter(hwnd);

            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(GetDlgItem(hwnd,IDC_LIST_OK), WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(GetDlgItem(hwnd,IDC_LIST_CANCEL), WM_SETFONT, (WPARAM)hfont0, FALSE);
            break;
        }

		case WM_COMMAND:{
			switch(LOWORD(wParam)){
				case IDC_LIST_OK:{
					EndDialog(hwnd,0);
					break;
				}

				case IDC_LIST_CANCEL:{
					EndDialog(hwnd,0);
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
BOOL CALLBACK SetPreferenceProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_CONFIG_DONE:
					EndDialog (hwnd, 0);
				break;

				case ID_CONFIG_CANCEL:{
					EndDialog (hwnd, 0);
					break;
				}
			}
		break;

		case WM_INITDIALOG:{
			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(GetDlgItem(hwnd,IDC_CONFIG_DONE), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,ID_CONFIG_CANCEL), WM_SETFONT, (WPARAM)hfont0, FALSE);

			MoveToScreenCenter(hwnd);
			break;
		}
	}

	return FALSE;
}

void buildConnectionTree(HWND hwnd){
	RECT            rt;

	HINSTANCE hinst = mainModel->hInstance;
	GetWindowRect(hwnd,&rt);

	HWND hTreeHwnd = CreateWindowEx(0,"SysTreeView32",0,
		WS_CHILD |WS_BORDER | WS_VISIBLE | TVIF_TEXT | TVS_HASLINES  | TVS_LINESATROOT,
        	5,5,
			120,300,
            hwnd,NULL,hinst,0);

	HIMAGELIST hImageList=ImageList_Create(16,16,ILC_COLOR24|ILC_MASK,2,10);
	HBITMAP hBitmap = LoadBitmap(hinst,MAKEINTRESOURCE(IDB_CONNECTION));
	// ImageList_Add(hImageList,hBitmap,NULL);
	ImageList_AddMasked(hImageList, hBitmap, RGB(255,255,255));
	SendMessage(hTreeHwnd,TVM_SETIMAGELIST,0,(LPARAM)hImageList);


	///////////////////////////
	// AppView * view = mainModel->view;

	TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

    tvinsert.hParent = NULL;
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	tvinsert.item.iImage=0;
	tvinsert.item.iSelectedImage=1;
    tvinsert.item.pszText= "mmm";

	TreeNode * treeNode = buildTreeNode();
	treeNode->level = 1;
	tvinsert.item.lParam=(LPARAM)treeNode;

	HTREEITEM parent = (HTREEITEM)SendMessage(hTreeHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

	for(int ix = 0; ix < 10; ix ++){
        tvinsert.hParent = parent;
	    tvinsert.hInsertAfter=TVI_ROOT;
	    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
	    tvinsert.item.iImage=2;
	    tvinsert.item.iSelectedImage=2;
        tvinsert.item.pszText= "00000";
		
		(HTREEITEM)SendMessage(hTreeHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	}

	////////////////////////
}

BOOL CALLBACK conectionConfigDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam){
	char buff[MAX_PATH];

	switch(msg)
	{
		case WM_INITDIALOG:{
			HINSTANCE hInstance = mainModel->hInstance;
			LoadString(hInstance,IDS_CODEVIEW,buff,MAX_PATH);
			SendDlgItemMessage(hWin,IDC_LST_COLORS,LB_ADDSTRING,0,(LPARAM)buff);

			MoveToScreenCenter(hWin);

			///////////////////////////////////////////////////////////////////////

			buildConnectionTree(hWin);

			///////////////////////////////////////////////////////////////////////

			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(GetDlgItem(hWin,IDOK), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hWin,IDCANCEL), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hWin,IDC_LST_COLORS), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hWin,IDC_STATIC), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hWin,IDC_STATIC), WM_SETFONT, (WPARAM)hfont0, FALSE);

			break;
		}

		case WM_COMMAND:
			switch(wParam){
				case IDOK:
					EndDialog(hWin,0);
				break;

				case IDCANCEL:
					EndDialog(hWin,0);
				break;
			}
			break;

		case WM_PAINT:
			//hdc = BeginPaint(hWin,&ps);
			//Rectangle(hdc,200,40,260,60);
			//Rectangle(hdc,200,100,260,120);
			//EndPaint(hWin,&ps);
		break;

		case WM_CLOSE:
			EndDialog(hWin,0);
		break;
	}

	return FALSE;
}

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
		case WM_INITDIALOG:{
			MoveToScreenCenter(hwnd);

			HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
            SendMessage(GetDlgItem(hwnd,IDC_ABOUT_OK), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_URL), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_NAME), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_DESCRIPTION), WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(GetDlgItem(hwnd,IDC_ABOUT_IMAGE), WM_SETFONT, (WPARAM)hfont0, FALSE);
			break;
		}

		case WM_COMMAND:{
		    switch (LOWORD (wParam)){
		    	case IDC_ABOUT_OK:{
					EndDialog (hwnd, 0);
					break;
				}
		    }
			break;
		}

	    case WM_CLOSE:
			EndDialog(hwnd,0);
		break;
    }

    return FALSE;
}

BOOL CALLBACK SetTtlDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	BOOL lineTogo =0 ;
	RECT rect;
	int mresult =0;

	switch(message){
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDC_TTL_OK:
					mresult = GetDlgItemInt(hwnd,IDC_TTL_EDIT,&lineTogo,TRUE);

					if(lineTogo){
						char buff[256]={0};
						sprintf(buff,"size:%d",mresult);
						// MessageBox(hwnd,buff,"title",MB_OK);
					}else{
						break;
					}
					

					EndDialog (hwnd, 0);
				break;

				case IDC_TTL_CANCEL:{
					EndDialog (hwnd, 0);
					break;
				}
			}
		break;

		case WM_INITDIALOG:{
            MoveToScreenCenter(hwnd);
            HWND styledd   = CreateWindowEx(0, WC_STATIC, ("TTL:"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 5, 5, 40, 24, hwnd, (HMENU)0, mainModel->hInstance, 0);
			HWND ttlEdit   = CreateWindowEx(0, WC_EDIT,   ("-1"), WEDIS_EDIT_STYLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_TTL_EDIT, mainModel->hInstance, 0);    
            HWND okBtn     = CreateWindowEx(0, WC_BUTTON, ("OK"), WEDIS_PUSH_BUTTON_STYLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_TTL_OK, mainModel->hInstance, 0);  
            HWND cancelBtn = CreateWindowEx(0, WC_BUTTON, ("Cancel"), WEDIS_PUSH_BUTTON_STYLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_TTL_OK, mainModel->hInstance, 0); 

            HFONT hfont0   = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
			SendMessage(styledd, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(ttlEdit, WM_SETFONT, (WPARAM)hfont0, FALSE);
			SendMessage(cancelBtn, WM_SETFONT, (WPARAM)hfont0, FALSE);
            SendMessage(okBtn, WM_SETFONT, (WPARAM)hfont0, FALSE);

			GetWindowRect(hwnd,&rect);
			
			int width = rect.right-rect.left;
			MoveWindow(ttlEdit,40+5+5,5, width-5-40-5-5-5,24,FALSE);
			MoveWindow(cancelBtn,width - 60 -5 -5 ,34,60,24,FALSE);
			MoveWindow(okBtn,width - 5 - 60 -5 - 60 -5,34,60,24,FALSE);
		    break;
        }

        case WM_CLOSE:{
            EndDialog(hwnd,0);
            break;
        }
	}
	return FALSE;
}
