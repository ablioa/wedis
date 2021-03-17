#include "stringview.h"

#define IDB_FORMAT_STRING 5000
#define IDB_EXPORT_STRING 5001
#define IDB_DELELE_STRING 5002
#define IDB_MOVE_STRING   5003
#define IDB_WIDEN_STRING  5004

#define MIN_HEX_WIDTH     16
#define HEX_WIDTH_STEP    8
#define MAX_HEX_WIDTH     48

HWND buildStringToolBar(HWND parent){
    int buttonCount = 6;
	TBBUTTON tbtn[6] = {
        {(TB_DELETE_BUTTON), IDB_DELELE_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_MOVE_BUTTON), IDB_MOVE_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(0), 0, TBSTATE_ENABLED, TBSTYLE_SEP, {0}, 0, 0},
        {(TB_EXPORT_BUTTON), IDB_EXPORT_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_FORMAT_BUTTON), IDB_FORMAT_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
        {(TB_WIDEN_BUTTON),  IDB_WIDEN_STRING, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
    };

    return buildGeneralToolBar(parent,tbtn,buttonCount);;
}

int get_next_hex_width(StringViewModel * model){
    model->hex_width += HEX_WIDTH_STEP;

    if(model->hex_width > MAX_HEX_WIDTH){
        model->hex_width = MIN_HEX_WIDTH;
    }

    return model->hex_width;
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
            model->mode = TEXT;
            model->hex_width=16;

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

                case IDB_WIDEN_STRING:{
                    if(model->mode == BINARY){
                        get_next_hex_width(model);
                        char * content = model->data->bulk->content;
                        int len = model->data->bulk->length;
                        content = dump_text(model->data->bulk->content,len,model->hex_width);
                        SetWindowText(model->stringView,content);
                    }
                    break;
                }

				/** display data in hex dump format*/
                case IDB_FORMAT_STRING:{
                    char * content = model->data->bulk->content;
                    if(model->mode == TEXT){
                        model->mode = BINARY;
                        int len = model->data->bulk->length;
                        content = dump_text(model->data->bulk->content,len,model->hex_width);
                    }else{
                        model->mode = TEXT;
                    }

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

            char * text = model->data->bulk->content;
            if(model->mode == BINARY){
                text = dump_text(model->data->bulk->content,model->data->bulk->length,model->hex_width);
            }

			SetWindowText(model->stringView,text);
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

char * get_output_buffer(int len,int width){
    int bsize = (12+width+4+width*3+2) * (len / width +1) +100;
    char * buff = ( char *) malloc(sizeof( char) * bsize);
    memset(buff,0,sizeof( char) * bsize);
    return buff;
}

char * dump_text(char * text,int len,int width){
    char * line   = (char*)calloc(1,width+1);//={0};
    char * buff   = get_output_buffer(len,width);
    char * output = buff;

    int offset =0;
    int ix = 0;
    for(ix = 0; ix < len; ix ++){
		if(ix % width ==0){
			sprintf(output,"%010X: ",(ix/width));
			output += 12;
		}

        sprintf(output,"%02X ",(unsigned char)(text[ix]));
        output +=3;
        line[offset++]= isprint(text[ix])?text[ix]:'.';

        if(ix % width == (width-1)){
            sprintf(output,"    %s\r\n",line);
            offset=0;
            memset(line,0,(width+1));
            output += (width+4+2); // 16+4+2
        }
    }

    if(ix % width != 0x00){
		int left  = len % width;
		int vleft = width - left;

		for(int iz = 0; iz < vleft; iz ++){
		   sprintf(output,"   ");
		   output += 3;
		}

		sprintf(output,"    %s\r\n",line);
		offset=0;
		memset(line,0,(width+1));
		output += (width+4+2);
    }

	return buff;
}

