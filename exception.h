#ifndef exception_h
#define exception_h

#include <windows.h>
#include <commctrl.h>

#define ERR_NORMAL					0x0000
#define ERR_CANNOT_CONNECTED		0X0001
#define ERR_CANNOT_CREATESOCKET		0x0002
#define ERR_SENDDATA_FAILED			0x0003
#define ERR_LINK_BREAKDOWN			0x0004

typedef struct err_info{
	LONG ecode;
	char Messag[MAX_PATH];
	UINT Style;
}eMsg;

extern eMsg err_list[];

void DumpMessage(long Err_Code);

#endif

