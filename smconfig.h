#ifndef _smconfig_h
#define _smconfig_h

#include <windows.h>

class config{
	COLORREF SplitColor;
	COLORREF codeForeColor;
	COLORREF dataForeColor;
	COLORREF regForeColor;
	COLORREF stackForeColor;
	COLORREF codeBackColor;
	COLORREF dataBackColor;
	COLORREF regBackColor;
	COLORREF stackBackColor;
	COLORREF curline;
public:
	char lpFileName[MAX_PATH];
public:
	config();
	void GetConfig();
	void SaveConfig();
	LONG GetInt(LPSTR sectionname,LPSTR keyname,LONG defval);
	LONG SaveInt(LPSTR sectionname,LPSTR keyname,LONG val);
	void SaveString(LPSTR sectionname,LPSTR keyname,LPSTR str);
};

#endif