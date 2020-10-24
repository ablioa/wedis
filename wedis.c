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
    //char buff[256] = {0};
    //sprintf(buff,"value: |%s|",message);
	MessageBox(NULL,message,"TITLE",MB_OK);
}

void log_int_message(int value){
    char buff[256] = {0};
    sprintf(buff,"value: %d",value);
    MessageBox(NULL,buff,"TITLE",MB_OK);
}

char * getOutputBuffer(int size){
    int bsize = (size/16+1) * 70+1;
    char * buff = ( char *) malloc(sizeof( char) * bsize);
    memset(buff,0,sizeof( char) * bsize);
    return buff;
}

char * dumpText( char * text,int len){
    char line[17]={0};
    char * buff   = getOutputBuffer(len);
    char * output =buff;

    int offset =0;
    int ix = 0;
    for(ix = 0; ix < len; ix ++){
        sprintf(output,"%02X ",(unsigned char)(text[ix]));
        output +=3;
        line[offset++]= isprint(text[ix])?text[ix]:'.';

        if(ix % 0x10 == 0x0f){
            sprintf(output,"    %s\r\n",line);
            offset=0;

            memset(line,0,17);

            output +=22;
        }
    }

    if(ix % 0x10 != 0x00){
        sprintf(output,"[ix=%d]",ix);
        for(ix =(0x10 - len % 0x10); ix < 0x10;ix++){
            sprintf(output,"   ");
        }

        sprintf(output,"    %s\r\n",line);
        output +=22;
    }

	return buff;
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

    //fprintf(log_file,"%s %s\n", log_time_stamp,buffer);
    fprintf(log_file,"%s\n", buffer);
    fflush(log_file);

    fprintf(stdout,"%s\n", buffer);
    
    return(cnt);
}