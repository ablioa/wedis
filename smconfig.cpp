#include "smconfig.h"
#define INI_NAME ".\\simu51_cfg.ini"

config::config()
{
	
}

void config::SaveConfig()
{
	//SaveInt("Chip","FLASH",0X2000);
	//SaveInt("Chip","RAM",0X100);
	//SaveInt("Chip","XALS",12000000);
	//SaveInt("Chip","XDATA",0X10000);
	//
	//SaveString("Chip","NAME","AT89C51");
	//SaveInt("Chip","FLASH",100);
	//SaveInt("Chip","FLASH",100);
	//SaveInt("Chip","FLASH",100);
}

LONG config::GetInt(LPSTR sectionname,LPSTR keyname,LONG defval)
{
	return GetPrivateProfileInt(sectionname,keyname,defval,lpFileName);
}

LONG config::SaveInt(LPSTR sectionname,LPSTR keyname,LONG val)
{
	char tmp[MAX_PATH];

	wsprintf(tmp,"%d",val);
	WritePrivateProfileString(sectionname,keyname,tmp,INI_NAME);

	return 0;
}

void config::SaveString(LPSTR sectionname,LPSTR keyname,LPSTR str)
{
	WritePrivateProfileString(sectionname,keyname,str,INI_NAME);
}