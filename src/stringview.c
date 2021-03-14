#include "stringview.h"

#define IDB_FORMAT_STRING 5000
#define IDB_EXPORT_STRING 5001
#define IDB_DELELE_STRING 5002
#define IDB_MOVE_STRING   5003

HWND buildStringToolBar(HWND parent){
	HINSTANCE hInst = App->hInstance;
	DWORD tstyle = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;

    int buttonCount = 5;
	TBBUTTON tbtn[5] = {
        {(0), IDB_DELELE_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(3), IDB_MOVE_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(0), 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
        {(1), IDB_EXPORT_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(2), IDB_FORMAT_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR_STRINGTB));
    HIMAGELIST hIcons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, buttonCount);
	ImageList_AddMasked(hIcons, hBmp, RGB(255,255,255));

    HWND tb = CreateWindowEx(0L, TOOLBARCLASSNAME, "", tstyle, 16, 16, 16, 16, parent, (HMENU) 0, hInst, NULL);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_SETIMAGELIST, 0, (LPARAM) hIcons);
    SendMessage(tb, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(tb, TB_ADDBUTTONS,       (WPARAM)buttonCount,       (LPARAM)&tbtn);
    SendMessage(tb, TB_AUTOSIZE, 0, 0);

    ShowWindow(tb,  TRUE);

    return tb;
}

LRESULT CALLBACK StringViewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect;
    StringViewModel * model = (StringViewModel *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch(message){
	    case WM_CREATE:{
            model = (StringViewModel*)malloc(sizeof(StringViewModel));
            memset(model,0,sizeof(StringViewModel));
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)model);

			model->stringView = CreateWindowEx(0, WC_EDIT, (""), 
					WS_VISIBLE | ES_AUTOVSCROLL | WS_BORDER | WS_CHILD | WS_TABSTOP | WS_VSCROLL |ES_MULTILINE, 
					0, 0, 0, 0, hwnd, (HMENU)IDC_STRING_VIEW_TEXT, App->hInstance, 0);
            model->toolBar = buildStringToolBar(hwnd);
            model->mode = BINARY;
            SendMessage(model->stringView, WM_SETFONT, (WPARAM)(resource->fixedWidthFont), FALSE);
			
		    break;
		}
       
        case WM_COMMAND:{
            switch(LOWORD (wParam)){
				/** move data into another database */
				case WM_MOVEDATA_CMD:{
					// TODO add move data operation
					break;
				}

				/** export data */
                case IDB_EXPORT_STRING:{
                    LPTSTR fileName = mGetSaveFileName(hwnd);
                    if(fileName == NULL){
                        break;
                    }
                    
                    FILE * stream = fopen(fileName,"wb");
                    fwrite(model->data->bulk->content,1,model->data->bulk->length,stream);
                    fclose(stream);
                    break;
                }

				/** display data in hex dump format*/
                case IDB_FORMAT_STRING:{
                    char * content = model->data->bulk->content;
                    if(model->mode == TEXT){
                        model->mode = BINARY;
                    }else{
                        int len = model->data->bulk->length;
                        content = dump_text(model->data->bulk->content,len);
                        model->mode = TEXT;
                    }

                    SendMessage(model->stringView,EM_REPLACESEL,FALSE,(LPARAM)content);
			        SetWindowText(model->stringView,content);
                    
                    break;
                }

				/** delete data item */
				case IDB_DELELE_STRING:{
					MessageBox(hwnd,"deleting string value now!","title",MB_OK);
					break;
			    }

				/** trigger move data menu */
				case IDB_MOVE_STRING:{
					POINT pt;
					GetCursorPos(&pt);
					TrackPopupMenu(App->activeHost->host->db_menu,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);
					break;
				}
            }
            break;
        }

		case WM_DT:{
            model->data = (RedisReply)wParam;
			model->dataNode = (TreeNode*)lParam;

			SendMessage(model->stringView,EM_REPLACESEL,FALSE,(LPARAM)model->data->bulk->content);
			SetWindowText(model->stringView,model->data->bulk->content);
            break;
        }

		case WM_SIZE:{
			GetClientRect(hwnd,&rect);
            MoveWindow(model->toolBar,0,0,rect.right-rect.left,28,TRUE);
			MoveWindow(model->stringView,0,28,rect.right-rect.left,rect.bottom-rect.top-28,TRUE);
		    break;
		}
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

void init_stringview(HINSTANCE hInstance){
	WNDCLASSEX stringViewClass;

    stringViewClass.cbSize        = sizeof(WNDCLASSEX);
    stringViewClass.style         = 0;
    stringViewClass.lpfnWndProc   = StringViewWndProc;
    stringViewClass.cbClsExtra    = 0;
    stringViewClass.cbWndExtra    = 0;
    stringViewClass.hInstance     = hInstance;
    stringViewClass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    stringViewClass.hCursor       = LoadCursor (hInstance, IDC_ARROW);
    stringViewClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    stringViewClass.lpszMenuName  = 0;
    stringViewClass.lpszClassName = STRING_VIEW_CLASS;
    stringViewClass.hIconSm       = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_MAIN));
    RegisterClassEx(&stringViewClass);
}


char * get_output_buffer(int size){
    int bsize = (size/16+1) * 70+1;
    char * buff = ( char *) malloc(sizeof( char) * bsize);
    memset(buff,0,sizeof( char) * bsize);
    return buff;
}

char * dump_text( char * text,int len){
    char line[17]={0};
    char * buff   = get_output_buffer(len);
    char * output =buff;

    int offset =0;
    int ix = 0;
    for(ix = 0; ix < len; ix ++){
        sprintf(output,"%02X ",(unsigned char)(text[ix]));
        output +=3;
        line[offset++]= isprint(text[ix])?text[ix]:'.';

        if(ix % 0x10 == 0x0f){
            sprintf(output,"    %s\r\n",line);
            offset=0;

            memset(line,0,17);

            output +=22;
        }
    }

    if(ix % 0x10 != 0x00){
        sprintf(output,"[ix=%d]",ix);
        for(ix =(0x10 - len % 0x10); ix < 0x10;ix++){
            sprintf(output,"   ");
        }

        sprintf(output,"    %s\r\n",line);
        output +=22;
    }

	return buff;
}

