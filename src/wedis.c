#include "wedis.h"

eMsg err_list[]={
    {0X0000,"",0},
    {0X0001,"Can't connect to server!",MB_RETRYCANCEL},
    {0x0002,"Create Socket Failed!",MB_OK},
    {0x0003,"Send data failed!",MB_RETRYCANCEL},
    {0x0004,"TCP-Server have broken down!",MB_OK},
};

int DumpMessage(long Err_Code){
    MSGBOXPARAMS *param = (MSGBOXPARAMS *)calloc(1,sizeof(MSGBOXPARAMS));

    param->cbSize      = sizeof(MSGBOXPARAMS);
    param->hInstance   = GetModuleHandle(NULL);
    param->hwndOwner   = App->mainWindowHwnd;
    param->lpszCaption = "ERROR";
    param->lpszIcon    = MAKEINTRESOURCE(IDI_MAIN);
    param->dwStyle     = MB_OKCANCEL;    

    param->lpszText = err_list[Err_Code].Messag;
    param->dwStyle  = err_list[Err_Code].Style | MB_USERICON ;

    return MessageBoxIndirect(param);
}

void initResource(){
    resource = (SystemResource*)malloc(sizeof(SystemResource));
    memset(resource,0,sizeof(SystemResource));

    resource->ctrlFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE,
            1, 400, 0, 0, 0, ("Ms Shell Dlg"));

    resource->fixedWidthFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE,
            1, 400, 0, 0, 0, ("Consolas"));

    HINSTANCE hInst = App->hInstance;
    HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR));
    resource->icons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, TOTAL_ICON_COUNT);
    ImageList_AddMasked(resource->icons, hBmp, RGB(255,255,255));

    resource->brush = CreateSolidBrush(RGB(240,240,240));
    log_file = fopen("wedis.log","w");
}

void log_message(const char * message){
    MessageBox(NULL,message,"message",MB_OK);
}
