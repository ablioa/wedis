#ifndef _smconfig_h
#define _smconfig_h

#include <windows.h>
#include "json.h"

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

   struct wedis_host * head;
   struct wedis_host * tail;
}Config;

typedef struct wedis_preference{
    int db_scan_default;
}Preference;

extern Config * appConfig;
extern Preference * preference;

void add_co_host(Host * host);

void remove_host_config(int hostIndex);

void read_host_config(Json * node,Host * host);

void save_all();

void save_config();

void save_all_host_config();

void save_host_config(int index,Host * host);

Host * getHostByIndex(Config * config,int hostIndex);

char * fetch_text_from_file(const char * filename,size_t * length);

#endif
