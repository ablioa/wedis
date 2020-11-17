#include "wedis.h"

SystemResource * resource;

FILE * log_file;

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