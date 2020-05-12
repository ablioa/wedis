#ifndef _smconfig_h
#define _smconfig_h

#include <windows.h>

#define INI_NAME ".\\wedis.ini"

#define GENERAL_CONFIG "General"
#define TOTAL_HOST     "total_host"

#define CONFIG_HOST     "host"
#define CONFIG_PORT     "port"

typedef struct{
    char * host;
    int    port;
}Host;

typedef struct{
   int total_host;
   Host ** hosts;
}Config;

extern Config * appConfig;

void save_all();

#endif