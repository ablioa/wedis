#include "controller.h"

void handleRedisData(Task * task,RedisReply * data){
	data->dataKey = (char*) malloc(sizeof(char) * 256);
	memset(data->dataKey,0,sizeof(char) * 256);
	sprintf(data->dataKey,"%s",task->dataKey);

	switch(task->dataType){
		case REDIS_STRING:{
			if(data->type == REPLY_BULK){
				data->dataTypeName = (char*) malloc(sizeof(char) * 256);
				memset(data->dataTypeName,0,sizeof(char) * 256);
				sprintf(data->dataTypeName,"%s","STRING");

				data->dataType = task->dataType;
				SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
			}else{
				log_message("wrong string data format");
			}
			break;
		}

		case REDIS_LIST:{
			if(data->type == REPLY_MULTI){
				data->dataTypeName = (char*) malloc(sizeof(char) * 256);
				memset(data->dataTypeName,0,sizeof(char) * 256);
				sprintf(data->dataTypeName,"%s","LIST");

				data->dataType = task->dataType;
				SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
			}else{
				log_message("wrong list data format");
			}
			break;
		}

		case REDIS_HASH:{
			if(data->type == REPLY_MULTI){
				data->dataTypeName = (char*) malloc(sizeof(char) * 256);
				memset(data->dataTypeName,0,sizeof(char) * 256);
				sprintf(data->dataTypeName,"%s","HASH");

				data->dataType = task->dataType;
				SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
			}else{
				log_message("wrong hash data format");
			}
			break;			
		}

		case REDIS_SET:{
			if(data->type == REPLY_MULTI){
				data->dataTypeName = (char*) malloc(sizeof(char) * 256);
				memset(data->dataTypeName,0,sizeof(char) * 256);
				sprintf(data->dataTypeName,"%s","SET");

				data->dataType = task->dataType;
				SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
			}else{
				log_message("wrong hash data format");
			}
			break;			
		}

		case REDIS_ZSET:{
			if(data->type == REPLY_MULTI){
				data->dataTypeName = (char*) malloc(sizeof(char) * 256);
				memset(data->dataTypeName,0,sizeof(char) * 256);
				sprintf(data->dataTypeName,"%s","ZSET");

				data->dataType = task->dataType;
				SendMessage(mainModel->view->dataHwnd,WM_DT,(WPARAM)data,(LPARAM)(NULL));
			}else{
				log_message("wrong hash data format");
			}
			break;			
		}
	}
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

void addDataNode(RedisReply * rp){
    TVITEM ti = {0};
    ti.mask = TVIF_HANDLE | TVIF_PARAM;
    ti.cchTextMax = 128;
    ti.hItem = mainModel->selectedNode;
    TreeView_GetItem(mainModel->view->connectionHwnd, &ti);
    
    TreeNode * tnf = (TreeNode*) ti.lParam;
    for(int ix =0; ix < tnf->subHandleSize; ix ++){
        TreeView_DeleteItem(mainModel->view->connectionHwnd,tnf->subHandles[ix]);
    }
    
    tnf->subHandleSize= 0;
	tnf->database = mainModel->database;
    
    for(int ix =0;ix < rp->bulkSize; ix ++){
        TV_INSERTSTRUCT tvinsert;
        
        tvinsert.hParent = mainModel->selectedNode;
        tvinsert.hInsertAfter=TVI_LAST;
        tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        tvinsert.item.pszText = rp->bulks[ix];
        tvinsert.item.iImage=2;
        tvinsert.item.iSelectedImage=2;
        
        TreeNode * tn = buildTreeNode();
        tn->level = 3;
		tn->database = tnf->database;
        tvinsert.item.lParam= (LPARAM)tn;
        tnf->subHandleSize ++;
        tnf->subHandles[ix]=(HTREEITEM)SendMessage(mainModel->view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
    }
}

void addDatabaseNode(int dbCount){
    TV_INSERTSTRUCT tvinsert;
    
	AppView * view = mainModel->view;
    HTREEITEM parentHandle = mainModel->connection->hostHandle;
    
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));
	tvinsert.hInsertAfter=TVI_ROOT;
	tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;

	char * dbname = (char *)malloc(sizeof(char)*128);
	for(int ix =0; ix < dbCount;ix ++){
		memset(dbname,0,sizeof(char) * 128);
		sprintf(dbname,"db%d",ix);

		tvinsert.item.iImage=1;
		tvinsert.item.iSelectedImage=1;
		tvinsert.hParent=parentHandle;
		tvinsert.hInsertAfter=TVI_LAST;
		tvinsert.item.pszText= dbname;

		TreeNode * tn = buildTreeNode();
		tn->level = 2;
		tn->database = ix;
		tvinsert.item.lParam= (LPARAM)tn;
    
		SendMessage(view->connectionHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	}

	free(dbname);
}