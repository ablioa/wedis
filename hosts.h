#ifndef hosts_h
#define hosts_h

#include "main.h"
#include "config.h"

struct wedis_hosts_model{
    HWND      hostTree;
    HTREEITEM parentTree;
};

typedef struct wedis_hosts_model * HostModel;

void buildConnectionTree(HWND hwnd);
BOOL CALLBACK conectionConfigDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

#endif
