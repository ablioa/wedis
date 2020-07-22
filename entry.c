#include "entry.h"
#include "main.h"

void initDataTypeSelector(HWND viewTypeHwnd){
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
    } ITEMINFO;

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

BOOL CALLBACK entryDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	char buff[MAX_PATH];

	switch(msg){
		case WM_INITDIALOG:{

            HWND dataType = GetDlgItem(hwnd,IDC_DATATYPE);
            initDataTypeSelector(dataType);

			MoveToScreenCenter(hwnd);
			break;
		}

		case WM_COMMAND:
			switch(wParam){
				case IDOK:
					EndDialog(hwnd,0);
				break;

				case IDCANCEL:
					EndDialog(hwnd,0);
				break;
			}
			break;

		case WM_PAINT:
		break;

		case WM_CLOSE:
			EndDialog(hwnd,0);
		break;
	}

	return FALSE;
}