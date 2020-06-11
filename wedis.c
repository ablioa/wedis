#include "wedis.h"

SystemResource * resource;

void initResource(){
    resource = (SystemResource*)malloc(sizeof(SystemResource));
    memset(resource,0,sizeof(SystemResource));

    resource->ctrlFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Ms Shell Dlg"));
    resource->fixedWidthFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, ("Consolas"));;
}
