#include "controller.h"

void handleKeyspace(Keyspace keyspace){
	Keyspace * spaces = (Keyspace*)calloc(keyspace->count,sizeof(Keyspace *));
    Keyspace inode    = keyspace->next;

	int ix = 0;
    while(inode != NULL){
		spaces[ix++] = inode;
        inode = inode->next;
    }

	mainModel->keyspaces  = spaces;
	mainModel->spaceCount = keyspace->count;

	redis_database_count();
}

void handleRedisData(Task * task,RedisReply data){
	// data->dataKey = (char*) calloc(strlen(task->dataKey)+1,sizeof(char));
	// sprintf(data->dataKey,"%s",task->dataKey);

	// switch(task->dataType){
	// 	case REDIS_STRING:{
	// 		if(data->type == REPLY_BULK){
	// 			data->dataTypeName = (char*) malloc(sizeof(char) * 256);
	// 			memset(data->dataTypeName,0,sizeof(char) * 256);
	// 			sprintf(data->dataTypeName,"%s","STRING");

	// 			data->dataType = task->dataType;
	// 			SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
	// 		}else{
	// 			log_message("wrong string data format");
	// 		}
	// 		break;
	// 	}

	// 	case REDIS_LIST:{
	// 		if(data->type == REPLY_MULTI){
	// 			data->dataTypeName = (char*) malloc(sizeof(char) * 256);
	// 			memset(data->dataTypeName,0,sizeof(char) * 256);
	// 			sprintf(data->dataTypeName,"%s","LIST");

	// 			data->dataType = task->dataType;
	// 			SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
	// 		}else{
	// 			log_message("wrong list data format");
	// 		}
	// 		break;
	// 	}

	// 	case REDIS_HASH:{
	// 		if(data->type == REPLY_MULTI){
	// 			data->dataTypeName = (char*) malloc(sizeof(char) * 256);
	// 			memset(data->dataTypeName,0,sizeof(char) * 256);
	// 			sprintf(data->dataTypeName,"%s","HASH");

	// 			data->dataType = task->dataType;
	// 			SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
	// 		}else{
	// 			log_message("wrong hash data format");
	// 		}
	// 		break;
	// 	}

	// 	case REDIS_SET:{
	// 		if(data->type == REPLY_MULTI){
	// 			data->dataTypeName = (char*) malloc(sizeof(char) * 256);
	// 			memset(data->dataTypeName,0,sizeof(char) * 256);
	// 			sprintf(data->dataTypeName,"%s","SET");

	// 			data->dataType = task->dataType;
	// 			SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
	// 		}else{
	// 			log_message("wrong hash data format");
	// 		}
	// 		break;			
	// 	}

	// 	case REDIS_ZSET:{
	// 		if(data->type == REPLY_MULTI){
	// 			data->dataTypeName = (char*) malloc(sizeof(char) * 256);
	// 			memset(data->dataTypeName,0,sizeof(char) * 256);
	// 			sprintf(data->dataTypeName,"%s","ZSET");

	// 			data->dataType = task->dataType;
	// 			SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
	// 		}else{
	// 			log_message("wrong hash data format");
	// 		}
	// 		break;			
	// 	}

	// 	case REDIS_UNDEFINED:{
	// 		log_message("undefined data type");
	// 		break;
	// 	}
	// }
}

void handleDataType(Task * task,DataType dataType){
	switch (dataType){
		case REDIS_STRING:{
			redis_get_string(task->dataKey);
			break;
		}

		case REDIS_HASH:{
			redis_get_hash(task->dataKey);
			break;
		}

		case REDIS_LIST:{
			redis_get_list(task->dataKey);
			break;
		}

		case REDIS_SET:{
			redis_get_set(task->dataKey);
			break;
		}

		case REDIS_ZSET:{
			redis_get_zset(task->dataKey);
			break;
		}

		default:{
			log_message("undefined data type");
			break;
		}
	}
}

void add_data_node(TreeNode * dbnode,RedisReply data){
    for(int ix =0; ix < dbnode->subHandleSize; ix ++){
        TreeView_DeleteItem(mainModel->view->connectionHwnd,dbnode->subHandles[ix]);
    }
    
    dbnode->subHandleSize= 0;
	RedisReply cursor = data->bulks[0];
	RedisReply keydata   = data->bulks[1];

	int total = keydata->array_length;
	for(int ix = 0; ix < total; ix ++){
		RedisReply item = keydata->bulks[ix];

        TV_INSERTSTRUCT tvinsert;

        tvinsert.hParent = dbnode->handle;
        tvinsert.hInsertAfter=TVI_LAST;
        tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvinsert.item.cchTextMax = item->bulk->length;
        tvinsert.item.pszText    = item->bulk->content;
        tvinsert.item.iImage=2;
        tvinsert.item.iSelectedImage=2;
     
        TreeNode * tn = build_tree_node(NODE_LEVEL_DATA);
		tn->data->data_key = item->bulk->content;
		// tn->database = dbnode->database;
        tvinsert.item.lParam= (LPARAM)tn;
		tn->handle = (HTREEITEM)SendMessage(mainModel->view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

        dbnode->subHandleSize ++;
        dbnode->subHandles[ix] = tn->handle;
	}
}

Keyspace getKeyspaceInfo(char * dbname){
	Keyspace * spaces = mainModel->keyspaces;
	if(spaces == NULL){
		return NULL;
	}

	for(int ix = 0; ix< mainModel->spaceCount; ix ++){
		if(strcmp(spaces[ix]->name,dbname) == 0){
			return spaces[ix];
		}
	}

	return NULL;
}

void add_database_node(TreeNode * hostNode,int dbCount){
    TV_INSERTSTRUCT tvinsert;
	char dbname[128] = {0};
    
	AppView * view = mainModel->view;
    HTREEITEM parentHandle = hostNode->handle;

    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;

	char * showName = (char *)malloc(sizeof(char)*128);
	for(int ix =0; ix < dbCount;ix ++){
		memset(dbname,0,sizeof(char) * 128);
		sprintf(dbname,"db%d",ix);

		memset(showName,0,sizeof(char) * 128);
		Keyspace space = getKeyspaceInfo(dbname);
		int keyCount = 0;
		if(space != NULL){
			keyCount = space->keys;
		}
		
		sprintf(showName,"%s(%d)",dbname,keyCount);

		tvinsert.item.iImage=1;
		tvinsert.item.iSelectedImage=1;
		tvinsert.hParent= parentHandle;
		tvinsert.hInsertAfter=TVI_LAST;
		tvinsert.item.pszText= showName;

		TreeNode * tn = build_tree_node(NODE_LEVEL_DATABASE);
		tn->database = ix;
		tn->stream = hostNode->stream;
		tvinsert.item.lParam= (LPARAM)tn;
    
		tn->handle = (HTREEITEM)SendMessage(
			view->connectionHwnd,
			TVM_INSERTITEM,
			0,
			(LPARAM)&tvinsert);
	}

	SendMessage(mainModel->view->connectionHwnd,TVM_EXPAND,(WPARAM)TVE_TOGGLE,(LPARAM)(hostNode->handle));
}

