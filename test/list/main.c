#include <stdio.h>
#include "../../src/queue.h"

typedef struct tree_node{
    int index;
}TNode;

TNode * getTNode(int index){
    TNode * node = (TNode*) calloc(1,sizeof(TNode));
    node->index = index;
    return node;
}

/**
 * 节点打印
 */
void print_tnode(void * tnode){
    printf("- %2d ",((TNode *)tnode)->index);
}

/**
 * 节点匹配
 */
int match_tnode(void * tnode,void * target){
    if(((TNode *)tnode)->index == ((TNode *)target)->index){
        return 1;
    }
    return 0;
}

int main(int argc,char **argv){
    list mlist = build_list();
   
    TNode * target  = getTNode(17);
    TNode * target1 = getTNode(100);
    TNode * target2 = getTNode(101);

    add_node(mlist,getTNode(100));
    add_node(mlist,getTNode(2));
    add_node(mlist,getTNode(5));
    add_node(mlist,getTNode(6));
    add_node(mlist,getTNode(7));
    add_node(mlist,getTNode(1));
    add_node(mlist,getTNode(1));
    add_node(mlist,getTNode(17));
    add_node(mlist,getTNode(0));
    add_node(mlist,getTNode(101));

    print_list(mlist,print_tnode);

    TNode * get =(TNode *)find_from_list(mlist,match_tnode,target); 
    printf("found: %d\n",get->index);
    print_list(mlist,print_tnode);

    get =(TNode *)find_from_list(mlist,match_tnode,target1); 
    printf("found: %d\n",get->index);
    print_list(mlist,print_tnode);
    
    get =(TNode *)find_from_list(mlist,match_tnode,target2); 
    printf("found: %d\n",get->index);
    print_list(mlist,print_tnode);
    
    return 0;
}

