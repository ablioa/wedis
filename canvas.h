#ifndef _canvas_h
#define _canvas_h
#include <windows.h>

class Canvas
{
public:
    operator HDC () { return _hdc; }
	
    void Line (int x1, int y1, int x2, int y2){
        ::MoveToEx (_hdc, x1, y1, 0);
        ::LineTo (_hdc, x2, y2);
    }

    void Text (int x, int y, char const * buf, int cBuf){
        ::TextOut ( _hdc, x, y, buf, cBuf );
    }

    void Char (int x, int y, char c){
        ::TextOut (_hdc, x, y, &c, 1);
    }

protected:

    Canvas (HDC hdc): _hdc (hdc) {}

    HDC  _hdc;
};


// Use for painting in response to WM_PAINT

class PaintCanvas: public Canvas
{
public:
    PaintCanvas (HWND hwnd): Canvas (::BeginPaint (hwnd, &_paint)),_hwnd (hwnd){}

    ~PaintCanvas (){
        ::EndPaint(_hwnd, &_paint);
    }
 
protected:
                 
    PAINTSTRUCT _paint;
    HWND        _hwnd;
};

#endif
