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

typedef struct wedis_host{
    char * name;
    char * host;
    int    port;
    int    requirepass;
    char * password;
    char * description;

    int    hostIndex;

    struct wedis_host * next;
}Host;

typedef struct{
   int total_host;

   int seq;

   //Host ** hosts;

   struct wedis_host * head;
   struct wedis_host * tail;
}Config;

extern Config * appConfig;

void add_co_host(Host * host);

void remove_host_config(int hostIndex);

void read_host_config(int index,Host * host);

void save_all();

void save_config();

void save_all_host_config();

void save_host_config(int index,Host * host);

Host * getHostByIndex(Config * config,int hostIndex);

LONG WritePrivateProfileInt(LPSTR sectionname,LPSTR keyname,LONG val,LPCSTR file);

#endif