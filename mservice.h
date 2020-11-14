#ifndef mservice_h
#define mservice_h

#include <windows.h>
#include <commctrl.h>

#include "myconnection.h"
#include "redis.h"
#include "service.h"


typedef struct{
	int level;
	
	int database;

	char * key;

    HTREEITEM subHandles[1024 * 1024];

	RedisConnection stream;

	HTREEITEM handle;

    int  subHandleSize;
}TreeNode;


void s_auth(TreeNode * hodeNode,char * password);

void s_key_space(TreeNode * hodeNode);

void s_db_select(TreeNode * selected);

#endif
