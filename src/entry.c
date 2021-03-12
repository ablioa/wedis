#include "main.h"
#include "entry.h"

typedef struct {
    int iImage;
    int iSelectedImage;
    int iIndent;
    const char * pszText;
} ITEMINFO;

ITEMINFO IInf[] = {
    { 0, 0,  0, "Json"}, 
    { 0, 0,  0, "Binary"},
    { 0, 0,  0, "Text"} 
};

void initDataTypeSelector(HWND viewTypeHwnd){
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_USEREX_CLASSES;
    InitCommonControlsEx(&icex);

    COMBOBOXEXITEM cbei;
    int iCnt;

    // Set the mask common to all items.
    cbei.mask = CBEIF_TEXT | CBEIF_INDENT |
                CBEIF_IMAGE| CBEIF_SELECTEDIMAGE;

    for(iCnt=0; iCnt<3; iCnt++){
        char * buff = (char*)calloc(1,255);
        strcpy(buff,IInf[iCnt].pszText);

        cbei.iItem          = iCnt;
        cbei.pszText        = buff;
        cbei.cchTextMax     = sizeof(IInf[iCnt].pszText);
        cbei.iImage         = IInf[iCnt].iImage;
        cbei.iSelectedImage = IInf[iCnt].iSelectedImage;
        cbei.iIndent        = IInf[iCnt].iIndent;
        
		SendMessage(viewTypeHwnd,CBEM_INSERTITEM,0,(LPARAM)&cbei);
    }

	HIMAGELIST hImageList=ImageList_Create(14,14,ILC_COLOR|ILC_MASK,2,10);
	HBITMAP hBitmap = LoadBitmap(App->hInstance,MAKEINTRESOURCE(IDB_CHIP)); //

	ImageList_Add(hImageList,hBitmap,NULL);

    SendMessage(viewTypeHwnd,CBEM_SETIMAGELIST,0,(LPARAM)hImageList);
}


BOOL CALLBACK entryDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	switch(msg){
		case WM_INITDIALOG:{
			MoveToScreenCenter(hwnd);
			break;
		}

		case WM_COMMAND:{
			switch(wParam){
				case IDOK:
					MessageBox(hwnd,"Hello,Message!!","Title",MB_OK);
					EndDialog(hwnd,0);
				break;
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

BOOL CALLBACK dbSearchDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    char page[MAX_PATH] = {0};
    char pattern[MAX_PATH] = {0};
    char psize[MAX_PATH] = {0};

	switch(msg){
		case WM_INITDIALOG:{
			MoveToScreenCenter(hwnd);
			break;
		}

		case WM_COMMAND:{
			switch(wParam){
				case IDC_DB_SEARCH_OK:
			        GetDlgItemText(hwnd,IDE1,page,MAX_PATH);
			        GetDlgItemText(hwnd,IDE2,pattern,MAX_PATH);
			        GetDlgItemText(hwnd,IDE3,psize,MAX_PATH);

                    char buff[1000] = {};
                    sprintf(buff,"scan %s %s %s",page,pattern,psize);

                    // TODO 在这里引入查询参数
					MessageBox(hwnd,buff,"search",MB_OK);
					EndDialog(hwnd,0);
				break;

                case IDC_DB_SEARCH_CANCEL:{
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

