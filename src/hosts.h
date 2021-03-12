#ifndef hosts_h
#define hosts_h

#include "main.h"
#include "config.h"

struct wedis_hosts_model{
    HWND      hostTree;
    HTREEITEM parentTree;
    HTREEITEM selectedTree;

    int       hostIndex;

    WNDPROC   hostEditProc;
    WNDPROC   nameEditProc;
    WNDPROC   portEditProc;
    WNDPROC   passwordEditProc;
    WNDPROC   descEditProc;
};

typedef struct wedis_hosts_model * HostModel;

void buildConnectionTree(HWND hwnd);
BOOL CALLBACK conectionConfigDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

#endif
