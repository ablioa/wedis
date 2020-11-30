#include "exception.h"
#include "callbacks.h"
#include "resource/resource.h"

eMsg err_list[]={
	{0X0000,"",0},
	{0X0000,"TCP-Client have not connected to any Server!\nsdsdsd\nsdfsfd\ndfsdf",MB_RETRYCANCEL},
	{0x0001,"Create Socket Failed!",MB_OK},
	{0x0002,"Send data failed!",MB_RETRYCANCEL},
	{0x0002,"TCP-Server have broken down!",MB_OK},
};

void DumpMessage(long Err_Code){
    MSGBOXPARAMS *param = (MSGBOXPARAMS *)calloc(1,sizeof(MSGBOXPARAMS));

    param->cbSize      = sizeof(MSGBOXPARAMS);
    param->hInstance   = GetModuleHandle(NULL);
    param->hwndOwner   = NULL;
    param->lpszCaption = "ERROR";
    param->lpszIcon    = MAKEINTRESOURCE(IDI_MAIN);
    param->dwStyle     = MB_OKCANCEL;	

	param->lpszText = err_list[Err_Code].Messag;
	param->dwStyle  = err_list[Err_Code].Style | MB_USERICON ;

	MessageBoxIndirect(param);
}
