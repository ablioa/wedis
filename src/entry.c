#include "main.h"
#include "entry.h"

typedef struct {
    int iImage;
    int iSelectedImage;
    int iIndent;
    const char * pszText;
} ITEMINFO;

ITEMINFO IInf[] = {
    { 1, 4,  0, "Json"}, 
    { 2, 5,  0, "Binary"},
    { 3, 6,  0, "Text"} 
};

void AddItems(HWND hWndComboBox){
	TCHAR Planets[5][10] =  
	{
		TEXT("string"), 
		TEXT("list"), 
		TEXT("hash"), 
		TEXT("set"), 
		TEXT("zset"), 
	};
		   
	TCHAR A[16]; 
	int  k = 0; 

	memset(&A,0,sizeof(A));       
	for (k = 0; k <= 4; k += 1){
		wcscpy_s(A, sizeof(A)/sizeof(TCHAR),  (TCHAR*)Planets[k]);
		SendMessage(hWndComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) A); 
	}
	  
	SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}


BOOL CALLBACK entryDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
		case WM_CREATE:{
			MessageBox(hwnd,"hwnd created,,,","title",MB_OK);
			break;
		}

        case WM_INITDIALOG:{
			HWND chwnd = GetDlgItem(hwnd,IDC_DATATYPE);
            //initDataTypeSelector(chwnd);
			AddItems(chwnd);
            MoveToScreenCenter(hwnd);
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case IDOK:{
                    EndDialog(hwnd,0);
                    break;
                }

                case IDCANCEL:{
                    EndDialog(hwnd,0);
                    break;
                }

				case IDC_DATATYPE:{ 
					if(HIWORD(wParam) == CBN_SELCHANGE){
						HWND chwnd = GetDlgItem(hwnd,IDC_DATATYPE);
						int ix = SendMessage(chwnd,CB_GETCURSEL,0,0);
						char buff[255] = {0};
						sprintf(buff,"select: %d",ix);
						SetWindowText(hwnd,buff);
					}
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
