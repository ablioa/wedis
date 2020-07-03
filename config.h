#ifndef _smconfig_h
#define _smconfig_h

#include <windows.h>

#define INI_NAME ".\\wedis.ini"

#define GENERAL_CONFIG "General"
#define TOTAL_HOST     "total_host"

#define CONFIG_NAME        "name"
#define CONFIG_DESC        "description"
#define CONFIG_HOST        "host"
#define CONFIG_PORT        "port"
#define CONFIG_REQUIREPASS "requirepass"
#define CONFIG_PASSWORD    "password"

typedef struct{
    char * name;
    char * host;
    int    port;
    int    requirepass;
    char * password;
    char * description;

    int    hostIndex;
}Host;

typedef struct{
   int total_host;
   Host ** hosts;
}Config;

extern Config * appConfig;

void save_all();

void save_config();

Host * getHostByIndex(Config * config,int hostIndex);

#endif