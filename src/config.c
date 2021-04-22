#include "config.h"

#include <stdio.h>
#include <assert.h>

#define CONFIG_REQUIREPASS "requirepass"
#define CONFIG_PASSWORD    "password"

#define CONFIG_FILENAME "wedis.json"

Config * appConfig;

Preference * preference;

Host * getHostByIndex(Config * config,int hostIndex){
    Host * result = NULL;

    Host * start = appConfig->head;
    while(start != NULL){
        
        if(start->hostIndex == hostIndex){
            result = start;
            break;
        }

        start = start->next;
    }

    return result;
}

void add_co_host(Host * host){
    if(appConfig->tail == NULL){
        appConfig->tail = host;
        appConfig->head = host;
    }else{
        appConfig->tail->next = host;
        appConfig->tail = host;
    }

    host->hostIndex = appConfig->seq++;
    appConfig->total_host ++;
}

void remove_host_config(int hostIndex){
    Host * start = appConfig->head;

    Host * pre = NULL;
    while(start != NULL){

        if(start->hostIndex == hostIndex){
            if(pre == NULL){
                appConfig->head = start->next;
            }else{
                pre->next = start->next;
            }

            // TODO 释放被删除节点的内存

            appConfig->total_host --;
            break;
        }

        pre = start;
        start = start->next;
    }
}

void load_all(){
    appConfig = (Config*)calloc(1,sizeof(Config));

    appConfig->head = NULL;
    appConfig->tail = NULL;

    size_t length = 0;
    char * json_text = fetch_text_from_file((const char *)CONFIG_FILENAME,&length);
    Json * json = json_parse(json_text);

    preference = (Preference*) calloc(1,sizeof(Preference));
    Json * sd = json_get_object_item(json,"dbScanDefault");
    preference->db_scan_default = sd->valueint;

    Json * hosts = json_get_object_item(json,"hosts");
    if(hosts){
        int host_count = json_get_array_size(hosts);
        for(int ix = 0; ix < host_count; ix ++){
            Host * host = (Host *) calloc(1,sizeof(Host));
            Json * hitem = json_get_array_item(hosts,ix);
            read_host_config(hitem,host);
            add_co_host(host);
        }
    }
}

char * fetch_text_from_file(const char * filename,size_t * length){
    FILE * fin = NULL;
    char * buff = NULL;
    fin = fopen(filename,"rb");
    if(fin == NULL){
        perror("SCRIPT OPENNING FAILED");
        return 0;
    }

    fseek(fin,0,SEEK_END);
    *length = ftell(fin);
    fseek(fin,0,SEEK_SET);
    buff = (char *)malloc(*length+1);
    if(buff == NULL){
        perror("MEMORY ALLOCATION[BUFF] FAILED");
        return 0;
    }

    memset(buff,0,*length+1);
    fread(buff,1,*length,fin);
    fclose(fin);

    return buff;
}

void read_host_config(Json * node,Host * host){
    Json * item;

    item = json_get_object_item(node,"name");
    host->name=item->valuestring;

    item = json_get_object_item(node,"description");
    host->description = item->valuestring;

    item = json_get_object_item(node,"host");
    host->host = item->valuestring;

    item = json_get_object_item(node,"port");
    host->port = item->valueint;

    item = json_get_object_item(node,"password");
    host->password = item->valuestring;

    item = json_get_object_item(node,"requirepass");
    host->requirepass = item->valueint;
}

Json* save_preference(Json * parent){
    Json * pcount = json_create_number(preference->db_scan_default);
    json_add_item_to_object(parent,"dbScanDefault",pcount);
    return pcount;
}

void save_all_host_config(){
    Json * root  = json_create_object();
    save_preference(root);

    Host * host = appConfig->head;
    Json * host_array_item = json_create_array();
    while(host){
        Json * host_item  = json_create_object();

        Json * name_node = json_create_string(host->name);
        json_add_item_to_object(host_item,"name",name_node);

        Json * host_node = json_create_string(host->host);
        json_add_item_to_object(host_item,"host",host_node);

        Json * port_node = json_create_number(host->port);
        json_add_item_to_object(host_item,"port",port_node);

        Json * password_node = json_create_string(host->password);
        json_add_item_to_object(host_item,"password",password_node);

        Json * requirepass_node = json_create_bool(host->requirepass);
        json_add_item_to_object(host_item,"requirepass",requirepass_node);

        Json * description_node = json_create_string(host->description);
        json_add_item_to_object(host_item,"description",description_node);

        json_add_item_reference_to_array(host_array_item,host_item);

        host = host->next;
    }

    json_add_item_to_object(root,"hosts",host_array_item);

    char * jsonText = json_print(root);
    FILE * file = fopen(CONFIG_FILENAME,"w");
    fprintf(file,"%s",jsonText);
    fclose(file);
}

void save_all(){
    load_all();
}

