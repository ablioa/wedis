#include "split.h"
#include "main.h"

#include <stdio.h>

static WORD _dotPatternBmp[8] = { 
    0x00aa, 0x0055, 0x00aa, 0x0055, 
    0x00aa, 0x0055, 0x00aa, 0x0055
};

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

    ////////
    HWND parentHwnd = GetParent(hwnd);
    RECT parentRect;
    RECT splitRect;
    
    ////////

    GetWindowRect(parentHwnd, &parentRect);
    GetWindowRect(hwnd,&splitRect);

    int shadowTop    = splitRect.top - parentRect.top;
    int shadowHeight = splitRect.bottom - splitRect.top;

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

        case WM_LBUTTONUP:{
            if(fDragMode == FALSE){break;}
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            ClientToScreen(hwnd,&pt);

             GetWindowRect(parentHwnd,&rect);
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

            hdc = GetWindowDC(parentHwnd);
            DrawXorBar(hdc, oldx,shadowTop, SPLIT_WIDTH ,shadowHeight);
            ReleaseDC(parentHwnd, hdc);
        
            oldx = pt.x;
        
            fDragMode = FALSE;
        
            pt.x += parentRect.left;
            pt.y += parentRect.top;
        
            ScreenToClient(parentHwnd, &pt);
            nSplitterPos = pt.x;

            ClipCursor(NULL);
            ReleaseCapture();


			for(int ix = 0; ix < splitModel->leftSize; ix ++){
				HWND hwnd = splitModel->left[ix];
				GetWindowRect(hwnd,&rect1);

				pp.x = rect1.left;
				pp.y = rect1.top;
				ScreenToClient(parentHwnd,&pp);

				qq.x = rect1.right;
				qq.y = rect1.bottom;
				ScreenToClient(parentHwnd,&qq);

				MoveWindow(hwnd,pp.x,pp.y,nSplitterPos-pp.x,qq.y - pp.y,TRUE);
                SendMessage(hwnd,WM_SIZE,NULL,NULL);
			}
			
			for(int ix = 0; ix < splitModel->rightSize; ix ++){
				HWND hwnd = splitModel->right[ix];
				GetWindowRect(hwnd,&rect1);

				pp.x = rect1.left;
				pp.y = rect1.top;
				ScreenToClient(parentHwnd,&pp);

				qq.x = rect1.right;
				qq.y = rect1.bottom;
				ScreenToClient(parentHwnd,&qq);

				MoveWindow(hwnd,nSplitterPos+SPLIT_WIDTH,pp.y,
					qq.x - (nSplitterPos+SPLIT_WIDTH),
					qq.y - pp.y,TRUE);
                
                SendMessage(hwnd,WM_SIZE,NULL,NULL);
			}

            GetWindowRect(hwnd,&rect1);
            pp.x = rect1.left;
			pp.y = rect1.top;
			ScreenToClient(parentHwnd,&pp);

			qq.x = rect1.right;
			qq.y = rect1.bottom;
			ScreenToClient(parentHwnd,&qq);

			MoveWindow(hwnd,nSplitterPos,pp.y,SPLIT_WIDTH,qq.y-pp.y,TRUE);
        }
        break;

        case WM_MOUSEMOVE:{
            if(fDragMode == FALSE){
                 break;
            }

            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            ClientToScreen(hwnd, &pt);
            
            pt.x -= parentRect.left;
            pt.y -= parentRect.top;

            OffsetRect(&parentRect, -parentRect.left, -parentRect.top);
            if(pt.x < 0){pt.x = 0;}
            if(pt.x > parentRect.right - SPLIT_WIDTH) {
                pt.x = parentRect.right - SPLIT_WIDTH;
            }
            
            if(pt.x != oldx && wParam & MK_LBUTTON){
                hdc = GetWindowDC(parentHwnd);
                DrawXorBar(hdc, oldx,shadowTop,SPLIT_WIDTH, shadowHeight);
                DrawXorBar(hdc, pt.x,shadowTop,SPLIT_WIDTH, shadowHeight);
                ReleaseDC(parentHwnd, hdc);
                oldx = pt.x;
            }
        }
        break;

        case WM_LBUTTONDOWN:{
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);

            ClipCursor(&parentRect);

            hdc = GetWindowDC(parentHwnd);
            DrawXorBar(hdc, pt.x,shadowTop, SPLIT_WIDTH ,shadowHeight);
            ReleaseDC(parentHwnd, hdc);

            ClientToScreen(hwnd,&pt);
            pt.x -= parentRect.left;
            pt.y -= parentRect.top;
            OffsetRect(&parentRect,-parentRect.left,-parentRect.top);

            if(pt.x < 0) {pt.x = 0;}
            if(pt.x > parentRect.right - SPLIT_WIDTH){
                pt.x = parentRect.right - SPLIT_WIDTH;
            }
            
            fDragMode = TRUE;
            SetCapture(hwnd);

            oldx = pt.x;
        }
        break;

        
        case WM_DESTROY:{
            PostQuitMessage(0);
        }
        break;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

 void WINAPI DrawXorBar(HDC hdc, int x, int y, int width, int height){
     HBITMAP hbitmap;
     HBRUSH hbrush,hbrushOld;
  
     hbitmap = CreateBitmap(8,8,1,1,_dotPatternBmp);
     hbrush = CreatePatternBrush(hbitmap);
  
     SetBrushOrgEx(hdc,x,y,0);
     hbrushOld = (HBRUSH)SelectObject(hdc,hbrush);

     PatBlt(hdc,x,y,width,height,PATINVERT);
  
     SelectObject(hdc,hbrushOld);
  
     DeleteObject(hbrush);
     DeleteObject(hbitmap);
 }