#ifndef _common_h
#define _common_h

#include <windows.h>

class App
{
protected:
	HWND hwnd;
	OPENFILENAME * ofn;
	LPSTR fname;
public:
	App(HWND _hwnd);
	~App();
	LPTSTR GetOpenFileName();
	LPTSTR GetSaveFileName();
};
#endif
