#include "config.h"

#define CONFIG_REQUIREPASS "requirepass"
#define CONFIG_PASSWORD    "password"

Config * appConfig;

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

    LONG total=GetPrivateProfileInt(GENERAL_CONFIG,TOTAL_HOST,0,INI_NAME);
    appConfig->total_host = 0;
    appConfig->seq = 0;

    for(int ix=0;ix < total; ix ++){
        Host * host = (Host *) calloc(1,sizeof(Host));
        read_host_config(ix,host);
        add_co_host(host);
    }
}

void read_host_config(int index,Host * host){
    char section_name[MAX_PATH]={0};
    wsprintf(section_name,"host/%d",index);

    host->hostIndex = index;
    
    host->name = (char *)calloc(MAX_PATH,sizeof(char));
    GetPrivateProfileString(section_name,CONFIG_NAME,"",host->name,MAX_PATH,INI_NAME);
    
    host->description = (char *)calloc(MAX_PATH,sizeof(char));
    GetPrivateProfileString(section_name,CONFIG_DESC,"",host->description,MAX_PATH,INI_NAME);
    
    host->host = (char *)calloc(MAX_PATH,sizeof(char));
    GetPrivateProfileString(section_name,CONFIG_HOST,"",host->host,MAX_PATH,INI_NAME);
    
    host->port=GetPrivateProfileInt(section_name,CONFIG_PORT,6379,INI_NAME);
    host->requirepass=GetPrivateProfileInt(section_name,CONFIG_REQUIREPASS,0,INI_NAME);
    
    host->password = (char *)calloc(MAX_PATH,sizeof(char));
    GetPrivateProfileString(section_name,CONFIG_PASSWORD,"",host->password,MAX_PATH,INI_NAME);
}

void save_host_config(int index,Host * host){
    char section_name[MAX_PATH]={0};
    wsprintf(section_name,"host/%d",index);
    
    WritePrivateProfileString(section_name,CONFIG_NAME,host->name,INI_NAME);
    WritePrivateProfileString(section_name,CONFIG_HOST,host->host,INI_NAME);
    WritePrivateProfileString(section_name,CONFIG_PASSWORD,host->password,INI_NAME);
    WritePrivateProfileString(section_name,CONFIG_DESC,host->description,INI_NAME);

    WritePrivateProfileInt(section_name,CONFIG_PORT,host->port,INI_NAME);
    WritePrivateProfileInt(section_name,CONFIG_REQUIREPASS,host->requirepass,INI_NAME);
}

void save_all_host_config(){
    WritePrivateProfileInt(GENERAL_CONFIG,TOTAL_HOST,appConfig->total_host,INI_NAME);

    Host * start = appConfig->head;
    while(start != NULL){
        save_host_config(start->hostIndex,start);
        start = start->next;
    }
}

void save_all(){
    load_all();
}

void save_config(){
    MessageBox(NULL,"service config!","title",MB_OK);    
}

LONG WritePrivateProfileInt(LPSTR sectionname,LPSTR keyname,LONG val,LPCSTR iniFile){
	char tmp[MAX_PATH];
	wsprintf(tmp,"%d",val);
	WritePrivateProfileString(sectionname,keyname,tmp,iniFile);
	return 0;
}
