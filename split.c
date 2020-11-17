#include "split.h"
#include "main.h"

#include <stdio.h>

static int  nSplitterPos = 100;
static int  oldy         = -2;
static int  oldx         = -2;

#define SPLIT_WIDTH 2

void initSplit(HINSTANCE hInstance){
    WNDCLASSEX vSplitClass;
    vSplitClass.cbSize        = sizeof(vSplitClass);
    vSplitClass.style         = 0;
    vSplitClass.lpfnWndProc   = vSplitProc;
    vSplitClass.cbClsExtra    = 0;
    vSplitClass.cbWndExtra    = 0;
    vSplitClass.hInstance     = hInstance;
    vSplitClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    vSplitClass.hCursor       = LoadCursor (NULL, IDC_SIZEWE);
    vSplitClass.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    vSplitClass.lpszMenuName  = 0;
    vSplitClass.lpszClassName = V_SPLIT;
    vSplitClass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);
    RegisterClassEx(&vSplitClass);
}

LRESULT CALLBACK vSplitProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	SplitModel * splitModel = (SplitModel*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
    static BOOL fDragMode = FALSE;

    POINT pt;
    HDC   hdc;
    RECT  rect;

    switch(msg){
        case WM_CREATE:{
            splitModel = (SplitModel*)calloc(1,sizeof(SplitModel));
			splitModel->leftSize  = 0;
			splitModel->rightSize = 0;

            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)splitModel);
            break;
        }

		case WM_SET_PANES_HWND:{
			splitModel->left[splitModel->leftSize++] = (HWND)wParam;
			splitModel->right[splitModel->rightSize++] = (HWND)lParam;
            break;
		}

        case WM_LBUTTONDOWN:{
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);

            RECT rectp;
            HWND parent = GetParent(hwnd);
            GetWindowRect(parent, &rectp);
            ClipCursor(&rectp);

            RECT rect1;
            POINT pp;
            POINT qq;

            GetWindowRect(hwnd,&rect1);
            pp.x = rect1.left;
			pp.y = rect1.top;
			ScreenToClient(parent,&pp);

			qq.x = rect1.right;
			qq.y = rect1.bottom;
			ScreenToClient(parent,&qq);

            hdc = GetWindowDC(parent);
            DrawXorBar(hdc, pt.x,pp.y, SPLIT_WIDTH ,qq.y-pp.y);
            ReleaseDC(parent, hdc);

            GetWindowRect(parent,&rect);
            
            ClientToScreen(hwnd,&pt);
            pt.x -= rect.left;
            pt.y -= rect.top;
            
            OffsetRect(&rect,-rect.left,-rect.top);
            
            if(pt.x < 0) {
                pt.x = 0;
            }

            if(pt.x > rect.right - SPLIT_WIDTH){
                pt.x = rect.right - SPLIT_WIDTH;
            }
            
            fDragMode = TRUE;
            SetCapture(hwnd);

            oldx = pt.x;
        }
        break;
       
        case WM_LBUTTONUP:{
             HWND parent = GetParent(hwnd);
             pt.x = (short)LOWORD(lParam);
             pt.y = (short)HIWORD(lParam);
        
             if(fDragMode == FALSE){
                 return 0;
             }
        
             GetWindowRect(parent,&rect);
             ClientToScreen(hwnd,&pt);
             pt.x -= rect.left;
             pt.y -= rect.top;
        
             OffsetRect(&rect,-rect.left,-rect.top);
        
             if(pt.x < 0) pt.x = 0;
             if(pt.x > rect.right - SPLIT_WIDTH) {
                 pt.x = rect.right - SPLIT_WIDTH;
             }

            RECT rect1;
			POINT pp;
			POINT qq;

            GetWindowRect(hwnd,&rect1);
            pp.x = rect1.left;
			pp.y = rect1.top;
			ScreenToClient(parent,&pp);

			qq.x = rect1.right;
			qq.y = rect1.bottom;
			ScreenToClient(parent,&qq);

            hdc = GetWindowDC(parent);
            DrawXorBar(hdc, oldx,pp.y, SPLIT_WIDTH ,qq.y-pp.y);
            ReleaseDC(parent, hdc);
        
             oldx = pt.x;
        
             fDragMode = FALSE;
        
             GetWindowRect(parent, &rect);
             pt.x += rect.left;
             pt.y += rect.top;
        
             ScreenToClient(parent, &pt);
             GetClientRect(parent, &rect);
             nSplitterPos = pt.x;

            ReleaseCapture();

            ClipCursor(NULL);
            ReleaseCapture();


			for(int ix = 0; ix < splitModel->leftSize; ix ++){
				HWND hwnd = splitModel->left[ix];
				GetWindowRect(hwnd,&rect1);

				pp.x = rect1.left;
				pp.y = rect1.top;
				ScreenToClient(parent,&pp);

				qq.x = rect1.right;
				qq.y = rect1.bottom;
				ScreenToClient(parent,&qq);

				MoveWindow(hwnd,pp.x,pp.y,nSplitterPos-pp.x,qq.y - pp.y,TRUE);
                SendMessage(hwnd,WM_SIZE,NULL,NULL);
			}
			
			for(int ix = 0; ix < splitModel->rightSize; ix ++){
				HWND hwnd = splitModel->right[ix];
				GetWindowRect(hwnd,&rect1);

				pp.x = rect1.left;
				pp.y = rect1.top;
				ScreenToClient(parent,&pp);

				qq.x = rect1.right;
				qq.y = rect1.bottom;
				ScreenToClient(parent,&qq);

				MoveWindow(hwnd,nSplitterPos+SPLIT_WIDTH,pp.y,
					qq.x - (nSplitterPos+SPLIT_WIDTH),
					qq.y - pp.y,TRUE);
                
                SendMessage(hwnd,WM_SIZE,NULL,NULL);
			}

            GetWindowRect(hwnd,&rect1);
            pp.x = rect1.left;
			pp.y = rect1.top;
			ScreenToClient(parent,&pp);

			qq.x = rect1.right;
			qq.y = rect1.bottom;
			ScreenToClient(parent,&qq);

			MoveWindow(hwnd,nSplitterPos,pp.y,SPLIT_WIDTH,qq.y-pp.y,TRUE);
        }
        break;

        case WM_MOUSEMOVE:{
            if(fDragMode == FALSE){
                 break;
            }

            HWND parent = GetParent(hwnd);
            
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            
            GetWindowRect(parent, &rect);
            
            ClientToScreen(hwnd, &pt);
            pt.x -= rect.left;
            pt.y -= rect.top;
            
            OffsetRect(&rect, -rect.left, -rect.top);
            
            if(pt.x < 0){
                pt.x = 0;
            }
            
            if(pt.x > rect.right - SPLIT_WIDTH) {
                pt.x = rect.right - SPLIT_WIDTH;
            }

            RECT rect1;
            POINT pp;
            POINT qq;

            GetWindowRect(hwnd,&rect1);
            pp.x = rect1.left;
			pp.y = rect1.top;
			ScreenToClient(parent,&pp);

			qq.x = rect1.right;
			qq.y = rect1.bottom;
			ScreenToClient(parent,&qq);

            char buff[1024] = {0};
            wsprintf(buff,"size: %d",pp.y);
            SetWindowText(parent,buff);

            RECT mrect;
            GetClientRect(hwnd,&mrect);

            if(pt.x != oldx && wParam & MK_LBUTTON){
                hdc = GetWindowDC(parent);
                DrawXorBar(hdc, oldx,mrect.top,SPLIT_WIDTH, qq.y-pp.y);
                DrawXorBar(hdc, pt.x,mrect.top,SPLIT_WIDTH, qq.y-pp.y);
                ReleaseDC(parent, hdc);
                oldx = pt.x;
            }
        }
        break;
        
        case WM_DESTROY:{
            PostQuitMessage(0);
        }
        break;
        
        default:
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

 void WINAPI DrawXorBar(HDC hdc, int x1, int y1, int width, int height){
     static WORD _dotPatternBmp[8] = { 
         0x00aa, 0x0055, 0x00aa, 0x0055, 
         0x00aa, 0x0055, 0x00aa, 0x0055
     };
  
     HBITMAP hbitmap;
     HBRUSH hbrush,hbrushOld;
  
     hbitmap = CreateBitmap(8,8,1,1,_dotPatternBmp);
     hbrush = CreatePatternBrush(hbitmap);
  
     SetBrushOrgEx(hdc,x1,y1,0);
  
     hbrushOld = (HBRUSH)SelectObject(hdc,hbrush);
  
     PatBlt(hdc,x1,y1,width,height,PATINVERT);
  
     SelectObject(hdc,hbrushOld);
  
     DeleteObject(hbrush);
     DeleteObject(hbitmap);
 }