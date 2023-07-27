#include "main.h"
#include "entry.h"
#include <windowsx.h>

#define IMPORT_STRING_DATA_BUTTON 9999
#define IMPORT_STRING_DATA_EDITOR 8888

const char * list_entry_column[2] = {"index","item"};
const char * set_entry_column[1]  = {"item"};
const char * hash_entry_column[2] = {"key","value"};
const char * zset_entry_column[2] = {"score","item"};

widget key_row[3] = {
    {NULL,{5, 5,-1,20},WC_EDIT,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,(HMENU)9},
    {NULL,{70,5,40,20},WC_BUTTON,"...",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)3},
    {NULL,{80,5,60,100},WC_COMBOBOXEX,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWN,(HMENU)IDC_DATATYPE}
};

widget entry_pane[5]={
    {NULL,{5,30,40,20},WDS_PANE,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP,(HMENU)1,stringEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,listEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,hashEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,setEntryEditorWndProc},
    {NULL,{5,30,40,20},WDS_PANE,"",WS_CHILD | WS_TABSTOP,(HMENU)1,zetEntryEditorWndProc}
};

/** string editor layout */
widget string_editor_pane[2]={
    {NULL,{0,0,200,20},TOOLBARCLASSNAME,"",WS_VISIBLE | WS_CHILD | TBSTYLE_TOOLTIPS|TBSTYLE_FLAT,(HMENU)IMPORT_STRING_DATA_BUTTON},
    {NULL,{0,25,100,100},WC_EDIT,"",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,(HMENU)IMPORT_STRING_DATA_EDITOR}
};

widget string_list_pane[1]={
    {NULL,{0,0,200,200},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT|LVS_SHAREIMAGELISTS,(HMENU)9}
};

widget string_hash_pane[1]={
    {NULL,{0,0,0,0},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_EDITLABELS,(HMENU)9}
};

widget string_set_pane[1]={
    {NULL,{0,0,0,0},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,(HMENU)9}
};

widget string_zset_pane[1]={
    {NULL,{0,0,0,0},"SysListView32","",WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,(HMENU)9}
};

BOOL init_list_column_name(HWND hWndListView,const char ** item,const int columns) {
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int i = 0; i < columns; i++){
        char * buff = calloc(1,255);
        strcpy(buff,item[i]);

        lvc.pszText  = buff;
        lvc.cx       = 100;
        lvc.iSubItem = i;
        lvc.fmt      = LVCFMT_LEFT;
        ListView_InsertColumn(hWndListView, i, &lvc);
    }

    return TRUE;
}

static void set_default_style(HWND hwnd){
    EnumChildWindows(hwnd,enumChildProc,0);
}

static void add_items(HWND hWndComboBox){
    const char * types[5]= {
		"- string",
		"- list",
		"- hash",
		"- set",
		"- zset"
	};

	for (int k = 0; k <= 4; k++){
		COMBOBOXEXITEM item;
		item.mask = CBEIF_TEXT | CBEIF_OVERLAY; //CBEIF_IMAGE|CBEIF_INDENT|CBEIF_OVERLAY|CBEIF_SELECTEDIMAGE|
		//item.iImage=1;
		//item.isSelectedImage=0;
		item.iOverlay =1;
		item.pszText = (LPTSTR)(LPCTSTR)(types[k]); //_T() 
		item.iItem = k;

        SendMessage(hWndComboBox,(UINT) CBEM_INSERTITEM,(WPARAM) 0,(LPARAM) &item); 
    }

    SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

LRESULT CALLBACK default_entry_proc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    return DefWindowProc(dataHwnd, msg, wParam, lParam);
}

int buttonCount = 1;
TBBUTTON string_toolbar_buttons[1] = {
    {(TB_ADD_BUTTON), TB_CMD_ADD_DATA, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
};

LRESULT CALLBACK stringEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    
    switch(msg){
        case WM_INIT_ENTRY:{
            GetClientRect(hwnd,&rect);
            for(int ix = 0; ix < 2; ix ++){
                widget * wgt = &string_editor_pane[ix];
                create_widget(hwnd,App->hInstance,wgt);
            }
        
            HWND tb = string_editor_pane[0].hwnd;
            SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) resource->icons);
            SendMessage(tb, TB_ADDBUTTONS,  (WPARAM)buttonCount,(LPARAM)string_toolbar_buttons);
            SendMessage(tb, TB_AUTOSIZE, 0, 0);

            MoveWindow(string_editor_pane[0].hwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,FALSE);
            MoveWindow(string_editor_pane[1].hwnd,0,28,rect.right-rect.left,rect.bottom-rect.top-28,FALSE);
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case TB_CMD_ADD_DATA:{
                    LPTSTR fileName = mGetOpenFileName(hwnd);
                    if(fileName == NULL){
                        break;
                    }
   
                    HWND parentHwnd = GetParent(hwnd);
                    SendMessage(parentHwnd,WM_DATA_VALUE,(WPARAM)NULL,(LPARAM)NULL);
                    break;
                }
            }

            switch(HIWORD(wParam)){
                case EN_CHANGE:{
                    char * buff = (char*) calloc(1,MAX_PATH);
                    int size = GetDlgItemText(hwnd,IMPORT_STRING_DATA_EDITOR,buff,MAX_PATH);
                    HWND parentHwnd = GetParent(hwnd);
                    SendMessage(parentHwnd,WM_DATA_VALUE,(WPARAM)buff,(LPARAM)size);
                    break;
                }
            }


            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK listEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_list_pane);
            HWND list_hwnd = string_list_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(list_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(list_hwnd,list_entry_column,2);

            GetClientRect(hwnd,&rect);
            MoveWindow(list_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK setEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_set_pane);
            HWND widget_hwnd = string_set_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(widget_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(widget_hwnd,set_entry_column,1);

            GetClientRect(hwnd,&rect);
            MoveWindow(widget_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK zetEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_zset_pane);
            HWND widget_hwnd = string_zset_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(widget_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(widget_hwnd,zset_entry_column,2);

            GetClientRect(hwnd,&rect);
            MoveWindow(widget_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK hashEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    RECT rect;
    switch(msg){
        case WM_INIT_ENTRY:{
            create_widget(hwnd,App->hInstance,string_hash_pane);
            HWND widget_hwnd = string_hash_pane[0].hwnd;
            ListView_SetExtendedListViewStyle(widget_hwnd,LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
            init_list_column_name(widget_hwnd,hash_entry_column,2);

            GetClientRect(hwnd,&rect);
            MoveWindow(widget_hwnd,0,0,rect.right-rect.left,rect.bottom - rect.top,TRUE);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/**
 * switch editor pane among difference data type.
 */ 
static void switch_editor_pane(int index){
    for(int ix = 0; ix < 5;ix ++){
        widget wgt = entry_pane[ix];
        if(ix == index){
            ShowWindow(wgt.hwnd,SW_SHOW);
        }else{
            ShowWindow(wgt.hwnd,SW_HIDE);
        }
    }
}

////////////////////
HWND hwndPB;
// HINSTANCE mhInstance;
HWND hwndStatic;

BOOL OnNotify(HWND hwndTab, HWND hwndDisplay, LPARAM lParam)
{
	//static int actualPage = 0;
    char achTemp[256]={0};

	switch (((LPNMHDR)lParam)->code)
		{
			case TCN_SELCHANGING:
				{
					// Return FALSE to allow the selection to change.
					return FALSE;
				}

			case TCN_SELCHANGE:
				{
                    HANDLE p = GetDlgItem(hwndTab, 0); 
                    int iPage = TabCtrl_GetCurSel(p); 

                    //MessageBox(hwndTab,"Hello,world.","xxx",MB_OK);
                    sprintf(achTemp,"index: %d",iPage);

                    printf("----- %d\n",iPage);

                    // Note that g_hInst is the global instance handle.
                    //LoadString(mhInstance, IDS_SUNDAY + iPage, achTemp,sizeof(achTemp) / sizeof(achTemp[0])); 
                    // LRESULT result = 
					SendMessage(hwndDisplay, WM_SETTEXT, 0, (LPARAM) achTemp); 

					//int iPage = TabCtrl_GetCurSel(hwndTab);
					//ShowWindow(hwndDisplay[actualPage], FALSE);
					//ShowWindow(hwndDisplay[iPage], TRUE);
					//actualPage = iPage;
					break;
				}
		}
		return TRUE;
}

HRESULT OnSize(HWND hwndTab, LPARAM lParam)
{
    //RECT rc; 

    if (hwndTab == NULL)
        return E_INVALIDARG;

    HANDLE p = GetDlgItem(hwndTab, 0); 

    // Resize the tab control to fit the client are of main window.
     if (!SetWindowPos(p, HWND_TOP, 0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), SWP_SHOWWINDOW))
        return E_FAIL;

    return S_OK;
}

BOOL CALLBACK frameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    RECT rcClient;
    //int cyVScroll;
    TCHAR achTemp[256]={0};
    TCITEM tie;

	switch(message){
        case WM_INITDIALOG:{
            GetClientRect(hwnd, &rcClient); 
    //        cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 

            hwndPB = CreateWindowEx(0, WC_TABCONTROL, (LPTSTR) NULL, 
                            WS_CHILD | WS_VISIBLE, 0, 
                            0, 
                            rcClient.right, rcClient.bottom, 
                            hwnd, (HMENU) 0, App->hInstance, NULL);

            hwndStatic= CreateWindow(WC_STATIC, "", 
              WS_CHILD | WS_VISIBLE | WS_BORDER, 
              100, 100, 100, 100,        // Position and dimensions; example only.
              hwndPB, NULL, App->hInstance,    // g_hInst is the global instance handle
              NULL); 


            tie.mask = TCIF_TEXT ; 
            tie.iImage = -1; 
            tie.pszText = achTemp; 

            for(int i = 0; i < 4; i++){
                LoadString(App->hInstance, IDS_DATA_SUNDAY + i, achTemp, sizeof(achTemp) / sizeof(achTemp[0])); 
                //TabCtrl_InsertItem(hwndPB, i, &tie);
				SendMessage(hwndPB, TCM_INSERTITEM, 0, (LPARAM) &tie);
            }

            EnumChildWindows(hwnd,enumChildProc,0);

			break;
        }

        case WM_SIZE:{
            OnSize(hwnd,lParam);
            break;
        }

        case WM_NOTIFY:{
            //LPNMHDR lParam = ((LPNMHDR) lParam);
            //MessageBox(hwnd,"ssss","sdsd",MB_OK);
            OnNotify(hwnd, hwndStatic,lParam);
            break;
        }

		case WM_COMMAND:{
			switch(LOWORD(wParam)){
				// case IDC_OK:{
                //     char buff[128] = {0};
                //     for(int ix = 0; ix < 100; ix ++){
                //         SendMessage(hwndPB, PBM_STEPIT, 0, 0);
                //     }

				// 	break;
				// }
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

/////////////////////

BOOL CALLBACK entryDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    static entry * ety;
    switch(msg){
        case WM_INITDIALOG:{
            ety = (entry*)lParam;

            RECT hrect;
            GetClientRect(hwnd,&hrect);
            for(int ix= 0; ix < 5; ix ++){
                widget * wgt = &entry_pane[ix];
                create_widget(hwnd,App->hInstance,wgt);

                dimension pos = wgt->position;
                pos.width  = hrect.right - hrect.left - 10;
                pos.height = hrect.bottom - hrect.top - 65;
                MoveWindow(wgt->hwnd,pos.left,pos.top,pos.width,pos.height,FALSE);

                SendMessage(wgt->hwnd,WM_INIT_ENTRY,0,0);
            }

            for(int ix = 0; ix < 3; ix ++){
                create_widget(hwnd,App->hInstance,&key_row[ix]);
            }

            HWND chwnd = GetDlgItem(hwnd,IDC_DATATYPE);
            add_items(chwnd);

            set_default_style(hwnd);
            arrange_widgets(hwnd,key_row,3);

            MoveToScreenCenter(hwnd);
            break;
        }

        case WM_DATA_KEY:{
            ety->key = (char *) wParam;
            ety->key_length = (int) lParam;
            break;
        }

        case WM_DATA_VALUE:{
            ety->string_data = (string_entry*) calloc(1,sizeof(string_entry));
            ety->string_data->data = (char *) wParam;
            ety->string_data->length = (int) lParam;
            break;
        }

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case IDOK:{
                    EndDialog(hwnd,0);
                    break;
                }

                case 3:{
                    DialogBox(App->hInstance,MAKEINTRESOURCE (IDD_DATA_FRAME),NULL,(DLGPROC)frameProc);
                    //LPTSTR file_name = mGetOpenFileName(hwnd);

                    //MessageBox(NULL,"Hello,world.","Caption",MB_OK);
                    //if(file_name == NULL){
                    //    break;
                    //}
              
                    //size_t len;
                    //char * key = fetch_text_from_file(file_name,&len);
                    //SendMessage(hwnd,WM_DATA_KEY,(WPARAM)key,(LPARAM)len);
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
                        switch_editor_pane(ix);
                    }
                    break;
                }
            }
            switch(HIWORD(wParam)){
                case EN_CHANGE:{
                    char * buff = (char*) calloc(1,MAX_PATH);
                    int size = GetDlgItemText(hwnd,9,buff,MAX_PATH);
                    SendMessage(hwnd,WM_DATA_KEY,(WPARAM)buff,(LPARAM)size);
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

void init_editor_view(HINSTANCE hInstance){
    WNDCLASSEX stringViewClass;

    stringViewClass.cbSize        = sizeof(WNDCLASSEX);
    stringViewClass.style         = 0;
    stringViewClass.cbClsExtra    = 0;
    stringViewClass.cbWndExtra    = 0;
    stringViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    stringViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    stringViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    stringViewClass.hbrBackground = resource->brush;
    stringViewClass.lpszMenuName  = 0;

    stringViewClass.lpszClassName = WDS_PANE;
    stringViewClass.lpfnWndProc   = default_entry_proc;
    stringViewClass.hInstance     = hInstance;

    RegisterClassEx(&stringViewClass);
}
