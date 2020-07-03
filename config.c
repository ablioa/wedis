#include "config.h"

#define CONFIG_REQUIREPASS "requirepass"
#define CONFIG_PASSWORD    "password"

Config * appConfig;

Host * getHostByIndex(Config * config,int hostIndex){
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
    appConfig = (Config*)calloc(1,sizeof(Config));

    LONG total=GetPrivateProfileInt(GENERAL_CONFIG,TOTAL_HOST,0,INI_NAME);
    appConfig->total_host = total;
    appConfig->hosts=(Host**)calloc(total,sizeof(Host*));

    for(int ix=0;ix < total; ix ++){
        Host * host = (Host *)calloc(1,sizeof(Host));

        char section_name[MAX_PATH]={0};
        wsprintf(section_name,"host/%d",ix);

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

        appConfig->hosts[ix] = host;
    }
}



void save_all(){
    load_all();
    //char tmp[MAX_PATH]={0};
    //wsprintf(tmp,"%d",10);
    //WritePrivateProfileString(GENERAL_CONFIG,TOTAL_HOST,tmp,INI_NAME);
}

void save_config(){
    MessageBox(NULL,"service config!","title",MB_OK);    
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