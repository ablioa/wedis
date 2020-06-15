#include "exception.h"

#include "callbacks.h"

#include "resource/resource.h"

eMsg err_list[]={
	{0X0000,"",0},
	{0X0000,"TCP-Client have not connected to any Server!",MB_RETRYCANCEL},
	{0x0001,"Create Socket Failed!",MB_OK},
	{0x0002,"Send data failed!",MB_RETRYCANCEL},
	{0x0002,"TCP-Server have broken down!",MB_OK},
};

void DumpMessage(long Err_Code){
    MSGBOXPARAMS *lpBoxParam = (MSGBOXPARAMS *)calloc(1,sizeof(MSGBOXPARAMS));

    lpBoxParam->cbSize      = sizeof(MSGBOXPARAMS);
    lpBoxParam->hInstance   = GetModuleHandle(NULL);
    lpBoxParam->hwndOwner   = NULL;
    lpBoxParam->lpszCaption = "ERROR";
    lpBoxParam->lpszIcon    = MAKEINTRESOURCE(ID_MAIN);
    lpBoxParam->dwStyle     = MB_OKCANCEL;	

	lpBoxParam->lpszText = err_list[Err_Code].Messag;
	lpBoxParam->dwStyle  = err_list[Err_Code].Style | MB_USERICON ;

	MessageBoxIndirect(lpBoxParam);
}
