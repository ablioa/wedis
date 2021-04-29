#include "dataview.h"

#define VIEW_SIZE 7

widget views[VIEW_SIZE]={
    {NULL,{0,0,0,0},HASH_VIEW_CLASS,    "",(!WS_VISIBLE) | WS_CHILD,(HMENU)6,NULL,REDIS_HASH},
    {NULL,{0,0,0,0},STRING_VIEW_CLASS,  "",(!WS_VISIBLE) | WS_CHILD,(HMENU)7,NULL,REDIS_STRING},
    {NULL,{0,0,0,0},LIST_VIEW_CLASS,    "",(!WS_VISIBLE) | WS_CHILD,(HMENU)8,NULL,REDIS_LIST},
    {NULL,{0,0,0,0},SET_VIEW_CLASS,     "",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_SET},
    {NULL,{0,0,0,0},ZSET_VIEW_CLASS,    "",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_ZSET},
    {NULL,{0,0,0,0},SYSTEM_VIEW_CLASS,  "",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_SYSTEM},
    {NULL,{0,0,0,0},DATABASE_VIEW_CLASS,"",(!WS_VISIBLE) | WS_CHILD,(HMENU)9,NULL,REDIS_DATABASE}
};

LRESULT CALLBACK dataViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    DataView * dataView = App->dataView;

    switch(msg){
        case WM_CREATE:{
            for(int i = 0; i < VIEW_SIZE; i ++){
                widget * wgt = &views[i];
                create_widget(hwnd,App->hInstance,wgt);
            }

            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)dataView);
            break;
        }

        case WM_DTT:{
            SendMessage(dataView->visibleHwnd,WM_DTT,wParam,lParam);
            break;
        }

        case WM_DT:{
            RedisReply data     = (RedisReply) wParam;
            TreeNode * datanode = (TreeNode *) lParam;

            int dataType = REDIS_SYSTEM;
            if(datanode->level == NODE_LEVEL_HOST){
                dataType = REDIS_SYSTEM;
            }else if(datanode->level == NODE_LEVEL_DATABASE){
                dataType = REDIS_DATABASE;
            }else{
                dataType = datanode->data->data_type;
            }

            switchView(hwnd,dataType,datanode,data);
            break;
        }

        case WM_SIZE:{
            RECT rect;
            GetClientRect(hwnd,&rect);
            for(int i = 0; i < VIEW_SIZE; i ++){
                widget * wgt = &views[i];
                MoveWindow(wgt->hwnd,0,0,rect.right-rect.left,rect.bottom-rect.top,TRUE);
            }
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void switchView(HWND hwnd,int type,TreeNode * treeNode,RedisReply data){
    for (int i = 0; i < 7; i++){
        int visible = SW_HIDE;
        if(views[i].type == type){
            visible = SW_SHOW;
            SendMessage(views[i].hwnd,WM_DT,(WPARAM)data,(LPARAM)treeNode);
        }

        ShowWindow(views[i].hwnd,visible);
    }

}
