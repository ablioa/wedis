#include "config.h"

Config * appConfig;

Host * getHostByIndex(Config * config,int hostIndex){
//    int cur = 0;
    Host * result = NULL;

    for(int cur = 0; cur < config->total_host; cur++){
        Host * host = config->hosts[cur];

        if(host->hostIndex == hostIndex){
            result = host;
            break;
        }
    }

    return result;
}

void load_all(){
    appConfig = (Config*)malloc(sizeof(Config));
    memset(appConfig,0,sizeof(Config));

    LONG total=GetPrivateProfileInt(GENERAL_CONFIG,TOTAL_HOST,0,INI_NAME);
    appConfig->total_host = total;

    appConfig->hosts=(Host**)malloc(sizeof(Host*)*total);
    memset(appConfig->hosts,0,sizeof(Host*)*total);

    for(int ix=0;ix < total; ix ++){
        Host * host = (Host *)malloc(sizeof(Host));
        memset(host,0,sizeof(Host));

        char section_name[MAX_PATH]={0};
        wsprintf(section_name,"host/%d",ix);

        char * host_name = (char *)malloc(sizeof(char) * MAX_PATH);
        memset(host_name,0,sizeof(char)*MAX_PATH);

        GetPrivateProfileString(section_name,CONFIG_HOST,"127.0.0.1",host_name,MAX_PATH,INI_NAME);
        host->host = host_name;

        LONG host_port=GetPrivateProfileInt(section_name,CONFIG_PORT,7333,INI_NAME);
        host->port=host_port;

        appConfig->hosts[ix] = host;
    }
}

void save_all(){
    load_all();
    //char tmp[MAX_PATH]={0};
    //wsprintf(tmp,"%d",10);
    //WritePrivateProfileString(GENERAL_CONFIG,TOTAL_HOST,tmp,INI_NAME);
}

//config::config(){
//	
//}
//
//void config::SaveConfig(){
//	//SaveInt("Chip","FLASH",0X2000);
//	//SaveInt("Chip","RAM",0X100);
//	//SaveInt("Chip","XALS",12000000);
//	//SaveInt("Chip","XDATA",0X10000);
//	//
//	//SaveString("Chip","NAME","AT89C51");
//	//SaveInt("Chip","FLASH",100);
//	//SaveInt("Chip","FLASH",100);
//	//SaveInt("Chip","FLASH",100);
//}

//LONG config::GetInt(LPSTR sectionname,LPSTR keyname,LONG defval){
//	return GetPrivateProfileInt(sectionname,keyname,defval,lpFileName);
//}
//
//LONG config::SaveInt(LPSTR sectionname,LPSTR keyname,LONG val){
//	char tmp[MAX_PATH];
//
//	wsprintf(tmp,"%d",val);
//	WritePrivateProfileString(sectionname,keyname,tmp,INI_NAME);
//
//	return 0;
//}
//
//void config::SaveString(LPSTR sectionname,LPSTR keyname,LPSTR str){
//	WritePrivateProfileString(sectionname,keyname,str,INI_NAME);
//}