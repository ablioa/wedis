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

list build_default_list();
void test_add_and_remove();
void test_find();

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

int match_by_value(void * tnode,void * target){
	if(((TNode *)tnode)->index == * ((int *)target)){
        return 1;
    }
    return 0;
}


int main(int argc,char **argv){
	// test_add_and_remove();

	test_find();
    return 0;
}

list build_default_list(){
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

	printf("==== list built ====\n");
	print_list(mlist,print_tnode);
	return mlist;
}

void test_add_and_remove(){
	list mlist = build_default_list();

    TNode * target  = getTNode(17);
    TNode * target1 = getTNode(100);
    TNode * target2 = getTNode(101);

    TNode * get =(TNode *)find_from_list(mlist,match_tnode,target); 
    printf("found: %d\n",get->index);
    print_list(mlist,print_tnode);

    get =(TNode *)find_from_list(mlist,match_tnode,target1); 
    printf("found: %d\n",get->index);
    print_list(mlist,print_tnode);
    
    get =(TNode *)find_from_list(mlist,match_tnode,target2); 
    printf("found: %d\n",get->index);
    print_list(mlist,print_tnode);
}

void test_find(){
	list mlist = build_default_list();
	int mvalue = 19;
	TNode * value = (TNode*)find(mlist,match_by_value,&mvalue);
	if(value !=NULL){
	    printf("found: %d\n",value->index);
	}else{
		printf("item not found.\n");
	}
    print_list(mlist,print_tnode);
}
