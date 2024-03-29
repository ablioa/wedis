#include "controller.h"

int MAX_DONE = 0;

void init_progress(int total){
    // set progress range
    SendMessage(App->view->progressBarHwnd,PBM_SETRANGE,0,MAKELPARAM(1,total));
}


/** update data transfer progress */
void update_transfer_progress(int done,int total){
   if(done > total){
        done = total;
    }

    if(done > MAX_DONE){
        char buff[128] = {0};
        sprintf(buff,"%d/%d",done,total);
        SendMessage(App->view->statusBarHwnd,SB_SETTEXT,5,(LPARAM)buff);
        MAX_DONE = done;
    }

    //if(done >= total){
    //    SendMessage(App->view->progressBarHwnd,PBM_SETPOS,(WPARAM)0,(LPARAM)0);
    //    return;
   // }

   // SendMessage(App->view->progressBarHwnd,PBM_SETRANGE,0,MAKELPARAM(1,total));
    //SendMessage(App->view->progressBarHwnd,PBM_SETPOS,(WPARAM)done,(LPARAM)0);
}

void handle_redis_data(TreeNode * datanode,RedisReply reply){
    // TODO handle wrong response
    // TODO release memory for the view
    SendMessage(App->view->dataviewHwnd,WM_DT,(WPARAM)reply,(LPARAM)(datanode));
}

TreeNode * add_host_node(char * host_name,TreeNode * hostNode){
    TV_INSERTSTRUCT tvinsert;
    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));

    //TreeNode * hostNode = build_tree_node(NULL,NODE_LEVEL_HOST);

    tvinsert.hParent             = NULL;
    tvinsert.hInsertAfter        = TVI_ROOT;
    tvinsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;
    tvinsert.item.iImage         = TREE_HOST_NODE;
    tvinsert.item.iSelectedImage = TREE_HOST_NODE;
    tvinsert.item.pszText        = host_name;
    tvinsert.item.lParam         = (LPARAM)hostNode;

    HTREEITEM  handle = (HTREEITEM)SendMessage(App->view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
    hostNode->handle = handle;

    return hostNode;
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

        item->bulk->content = item->bulk->content;
        item->bulk->length  = item->bulk->length;

        TreeNode * datanode = build_tree_node(dbnode,NODE_LEVEL_DATA);
        datanode->data->data_key   = item->bulk->content;
        datanode->data->key_length = item->bulk->length;
        datanode->stream = dbnode->stream;

        int dataType = TREE_DATA_NODE;
        DataType dt = s_db_get_data_type(dbnode->stream,item->bulk->content,item->bulk->length);

        if(dt == REDIS_STRING){
            dataType = TREE_DATA_NODE_TEXT;
        }else if(dt == REDIS_LIST){
            dataType = TREE_DATA_NODE_LIST;
        }else if (dt == REDIS_HASH){
            dataType = TREE_DATA_NODE_HASH;
        }else if (dt == REDIS_SET ){
            dataType = TREE_DATA_NODE_SET;
        }else if (dt == REDIS_ZSET){
            dataType = TREE_DATA_NODE_ZSET;
        }

        int tlen;
        char * encoded_key=encode(item->bulk->content,item->bulk->length,&tlen);

        TV_INSERTSTRUCT tvinsert;
        tvinsert.hParent             = dbnode->handle;
        tvinsert.hInsertAfter        = TVI_LAST;
        tvinsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        tvinsert.item.cchTextMax     = tlen;
        tvinsert.item.pszText        = encoded_key;
        tvinsert.item.iImage         = dataType;//TREE_DATA_NODE_HASH;
        tvinsert.item.iSelectedImage = dataType;// TREE_DATA_NODE_HASH;
        tvinsert.item.lParam         = (LPARAM)datanode;

        datanode->handle = (HTREEITEM)SendMessage(App->view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

        dbnode->subHandleSize ++;
        dbnode->subHandles[ix] = datanode->handle;

        free(encoded_key);
    }

    TreeView_Expand(App->view->overviewHwnd,dbnode->handle,TVE_EXPAND);
}

void add_database_node(TreeNode * hostNode,int dbCount){
    TV_INSERTSTRUCT tvinsert;
    
    AppView * view = App->view;
    HTREEITEM parentHandle = hostNode->handle;

    /** initialize connection specific data */
    hostNode->host->db_count = dbCount;

    memset(&tvinsert,0,sizeof(TV_INSERTSTRUCT));
    tvinsert.hInsertAfter=TVI_ROOT;
    tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE| TVIF_PARAM;

    for(int dbindex =0; dbindex < dbCount;dbindex ++){
        char showName[255]={0};
        TreeNode * dbnode = build_tree_node(hostNode,NODE_LEVEL_DATABASE);
        dbnode->database->dbindex=dbindex;
        sprintf(dbnode->database->dbname,"db%d",dbindex);
        dbnode->stream = hostNode->stream;
        
        sprintf(showName,"%s",dbnode->database->dbname);
        tvinsert.item.iImage         = TREE_DATABASE_NODE;
        tvinsert.item.iSelectedImage = TREE_DATABASE_NODE;
        tvinsert.hParent= parentHandle;
        tvinsert.hInsertAfter=TVI_LAST;
        tvinsert.item.pszText= showName;
        tvinsert.item.lParam= (LPARAM)dbnode;

        HTREEITEM handle = (HTREEITEM)SendMessage(view->overviewHwnd,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
        dbnode->handle = handle;

        /** add other database menu*/
        dbnode->database->db_menu  = CreatePopupMenu();
        for(int ix =0; ix < dbCount; ix ++){
            if(ix == dbindex){
                continue;
            }
            char dbname[128]={};
            sprintf(dbname,"db%d",ix);
            AppendMenu(dbnode->database->db_menu,MF_STRING,WM_MOVEDATA_CMD+ix,dbname);
        }
    }

    SendMessage(App->view->overviewHwnd,TVM_EXPAND,(WPARAM)TVE_TOGGLE,(LPARAM)(hostNode->handle));
}

