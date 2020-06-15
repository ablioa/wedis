#include "wedis.h"

SystemResource * resource;

void initResource(){
    resource = (SystemResource*)malloc(sizeof(SystemResource));
    memset(resource,0,sizeof(SystemResource));

    resource->ctrlFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
    resource->fixedWidthFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Consolas"));;
}

void log_message(const char * message){
	MessageBox(NULL,message,"TITLE",MB_OK);
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
    for(int ix = 0; ix < len; ix ++){
        sprintf(output,"%02X ",(unsigned char)(text[ix]));
        output +=3;
        line[offset++]= isprint(text[ix])?text[ix]:'.';

        if(ix % 0x10 == 0x0f){
            sprintf(output,"    %s\r\n",line);
            offset=0;

            output +=22;
        }
    }

	return buff;
}