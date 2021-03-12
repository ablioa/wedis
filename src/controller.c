#include "controller.h"

void update_progress(){
	
}

void handle_redis_data(TreeNode * datanode,RedisReply reply){
	SendMessage(App->view->dataviewHwnd,WM_DT,(WPARAM)reply,(LPARAM)(datanode));
}

void add_data_node(TreeNode * dbnode,RedisReply data){
    for(int ix =0; ix < dbnode->subHandleSize; ix ++){
        TreeView_DeleteItem(App->view->overviewHwnd,dbnode->subHandles[ix]);
    }

	RedisReply cursor = data->bulks[0];
	dbnode->database->cursor = atoi(cursor->bulk->content);
	SendMessage(App->view->dataviewHwnd,WM_DTT,(WPARAM)dbnode,(LPARAM)cursor->bulk->content);

    dbnode->subHandleSize= 0;
	RedisReply keydata   = data->bulks[1];

	int total = keydata->array_length;
	for(int ix = 0; ix < total; ix ++){
		RedisReply item = keydata->bulks[ix];

        TreeNode * datanode = build_tree_node(dbnode,NODE_LEVEL_DATA);
		datanode->data->data_key = item->bulk->content;
		datanode->stream = dbnode->stream;

        TV_INSERTSTRUCT tvinsert;
        tvinsert.hParent = dbnode->handle;
        tvinsert.hInsertAfter=TVI_LAST;
        tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvinsert.item.cchTextMax = item->bulk->length;
        tvinsert.item.pszText    = item->bulk->content;
        tvinsert.item.iImage=2;
        tvinsert.item.iSelectedImage=2;
        tvinsert.item.lParam= (LPARAM)datanode;
		datanode->handle = (HTREEITEM)SendMessage(App->view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

        dbnode->subHandleSize ++;
        dbnode->subHandles[ix] = datanode->handle;
	}

	TreeView_Expand(App->view->overviewHwnd,dbnode->handle,TVE_EXPAND);
}

void add_database_node(TreeNode * hostNode,int dbCount){
    TV_INSERTSTRUCT tvinsert;
    
	AppView * view = App->view;
    HTREEITEM parentHandle = hostNode->handle;

    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;

	char showName[255]={0};
	for(int dbindex =0; dbindex < dbCount;dbindex ++){
		TreeNode * dbnode = build_tree_node(hostNode,NODE_LEVEL_DATABASE);
		dbnode->database->dbindex=dbindex;
		sprintf(dbnode->database->dbname,"db%d",dbindex);
		dbnode->stream = hostNode->stream;

		memset(showName,0,sizeof(char) * 255);
		// Keyspace space = getKeyspaceInfo(dbnode->database->dbname);
		// int keyCount = 0;
		// if(space != NULL){
		// 	keyCount = space->keys;
		// }
		
		memset(showName,0,255);
		sprintf(showName,"%s",dbnode->database->dbname);
		tvinsert.item.iImage=1;
		tvinsert.item.iSelectedImage=1;
		tvinsert.hParent= parentHandle;
		tvinsert.hInsertAfter=TVI_LAST;
		tvinsert.item.pszText= showName;
		tvinsert.item.lParam= (LPARAM)dbnode;

		HTREEITEM handle = (HTREEITEM)SendMessage(view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
		dbnode->handle = handle;
	}

	SendMessage(App->view->overviewHwnd,TVM_EXPAND,(WPARAM)TVE_TOGGLE,(LPARAM)(hostNode->handle));
}

