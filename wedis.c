#include "wedis.h"

SystemResource * resource;

FILE * log_file;

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
    param->hwndOwner   = mainModel->mainWindowHwnd;
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

    resource->ctrlFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
    resource->fixedWidthFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Consolas"));

    log_file = fopen("wedis.log","w");
}

void log_message(const char * message){
	MessageBox(NULL,message,"message",MB_OK);
}

int wedis_log(const char *fmt, ...){
    static char   buffer[BUFF_SIZE];
    static const char * time_fmt = "[%04d-%02d-%02d %02d:%02d:%02d]";

    time_t sec;
    char log_time_stamp[25];
    struct tm * target_time;

    time(&sec);
    target_time = localtime(&sec);
    memset(log_time_stamp,0,25);

    sprintf(log_time_stamp,time_fmt,
            target_time->tm_year+1900,
            target_time->tm_mon+1,
            target_time->tm_mday,
            target_time->tm_hour,
            target_time->tm_min,
            target_time->tm_sec);

    va_list argptr;
    int cnt;
    va_start(argptr, fmt);
    cnt = vsnprintf(buffer,BUFF_SIZE ,fmt, argptr);
    va_end(argptr);

    fprintf(log_file,"%s\n", buffer);
    fflush(log_file);

    fprintf(stdout,"%s\n", buffer);
    
    return(cnt);
}