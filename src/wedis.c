#include "wedis.h"

#define MSG_CONNECT_FAILED       0x00001 /* Can't connect to server! */
#define MSG_SOCKET_FAILED        0x00002 /* Create Socket Failed!    */
#define MSG_SENT_FAILED          0x00003 /* Send data failed!        */
#define MSG_SERVER_BROKEN        0x00004 /* TCP-Server have broken down! */
#define MSG_WINDOW_BUILD_FAILED  0x00005 /* Create Window failed. */
#define MSG_FLUSHALL_WARNIG      0x00006 /* Flushall will remove all databases,continiue? */
#define MSG_FLUSHDB_WARNING      0x00007 /* Do you want to remove the data item? */
#define MSG_REMOVE_DATA_WARNING  0x00008 /* Flushdb will remove all data in the database,continue. */
#define MSG_DATA_REMOVED_INFO    0x00009 /* Data does not exist anymore.*/

/** define string table resource for msg definition. */
#define IDS_MSG_CONNECT_FAILED       0x00001 /* Can't connect to server! */
#define IDS_MSG_SOCKET_FAILED        0x00002 /* Create Socket Failed!    */
#define IDS_MSG_SENT_FAILED          0x00003 /* Send data failed!        */
#define IDS_MSG_SERVER_BROKEN        0x00004 /* TCP-Server have broken down! */
#define IDS_MSG_WINDOW_BUILD_FAILED  0x00005 /* Create Window failed. */
#define IDS_MSG_FLUSHALL_WARNIG      0x00006 /* Flushall will remove all databases,continiue? */
#define IDS_MSG_FLUSHDB_WARNING      0x00007 /* Do you want to remove the data item? */
#define IDS_MSG_REMOVE_DATA_WARNING  0x00008 /* Flushdb will remove all data in the database,continue. */
#define IDS_MSG_DATA_REMOVED_INFO    0x00009 /* Data does not exist anymore.*/

/** user message structure */
typedef struct{
	unsigned int msgId;
	unsigned int resourceId;
	UINT msgStyle;
}UserMessage;

/** predefined user message pool */
UserMessage messagePool[]={
	{MSG_CONNECT_FAILED,222},
	{MSG_SOCKET_FAILED,222},
	{MSG_SENT_FAILED,222},
	{MSG_SERVER_BROKEN,222},
	{MSG_WINDOW_BUILD_FAILED,222},
	{MSG_FLUSHALL_WARNIG,222},
	{MSG_FLUSHDB_WARNING,222},
	{MSG_REMOVE_DATA_WARNING,222},
	{MSG_DATA_REMOVED_INFO,222}
};

UserMessage * getUserMessage(){
	return NULL;
}

eMsg err_list[]={
    {0X0000,"",0},
    {0X0001,"Can't connect to server!",MB_RETRYCANCEL},
    {0x0002,"Create Socket Failed!",MB_OK},
    {0x0003,"Send data failed!",MB_RETRYCANCEL},
    {0x0004,"TCP-Server have broken down!",MB_OK},
    {0x0005,"Create Window failed.",MB_OK},
    {0x0006,"Flushall will remove all databases,continiue?",MB_YESNOCANCEL|MB_ICONASTERISK},
    {0x0007,"Do you want to remove the data item?",MB_YESNOCANCEL|MB_ICONASTERISK},
    {0x0008,"Flushdb will remove all data in the database,continue.",MB_YESNOCANCEL|MB_ICONASTERISK},
    {0x0009,"Data does not exist anymore.",MB_OK}
};

int DumpMessage(long Err_Code){
    MSGBOXPARAMS *param = (MSGBOXPARAMS *)calloc(1,sizeof(MSGBOXPARAMS));

    param->cbSize      = sizeof(MSGBOXPARAMS);
    param->hInstance   = GetModuleHandle(NULL);
    param->hwndOwner   = App->mainWindowHwnd;
    param->lpszCaption = "ERROR";
    param->lpszIcon    = MAKEINTRESOURCE(IDI_MAIN);
    param->dwStyle     = MB_OKCANCEL;

    param->lpszText = err_list[Err_Code].Messag;
    param->dwStyle  = err_list[Err_Code].Style | MB_USERICON ;

    return MessageBoxIndirect(param);
}

void initResource(){
    resource = (SystemResource*)malloc(sizeof(SystemResource));
    memset(resource,0,sizeof(SystemResource));

    resource->ctrlFont = CreateFont(
		    16, 
		    0, 
		    0, 
		    0, 
		    400, 
		    FALSE,
			FALSE, 
			FALSE,
            1, 
			400, 
			0, 
			0, 
			0, 
			("Ms Shell Dlg"));

    resource->fixedWidthFont = CreateFont(
			16,
			0,
			0,
			0,
			400, FALSE, FALSE, FALSE,
			1,
			400,
			0,
			0,
			0,
			("Consolas"));

    HINSTANCE hInst = App->hInstance;
    HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TOOLBAR));
    resource->icons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, TOTAL_ICON_COUNT);
    ImageList_AddMasked(resource->icons, hBmp, RGB(255,255,255));

    resource->brush = CreateSolidBrush(RGB(240,240,240));
    log_file = fopen("wedis.log","w");
}

void log_message(const char * message){
}
