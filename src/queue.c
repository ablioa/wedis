#include "queue.h"

/* build a element of list */
list_node build_list_node(void * value){
    list_node node = (list)calloc(1,sizeof(struct wedis_list));

    node->value = value;
    node->next = NULL;
    node->prev = NULL;

    return node;
}
/* build a new list */
list build_list(){
    list_node node = build_list_node(NULL);
    node->size = 0;
    node->next = NULL;
    node->prev = NULL;

    return (list)node;
}

void add_node(list head,void * value){
    list new_node = build_list_node(value);

    list cur = head;
    while(cur->next != NULL){
        cur =cur->next;
    }

    cur->next      = new_node;
    new_node->prev = cur;

    head->size ++;
}

/* find_then_remove */
void * find_from_list(list head,node_match match,void * param){
    list cur = head->next;

    while(cur != NULL){
        if(match(cur->value,param) == 1){

            cur->prev->next = cur->next;
            if(cur->next != NULL){
                cur->next->prev = cur->prev;
            }

            head->size --;

            void * val = cur->value;
            free(cur);
            return val;
        }

        cur = cur->next;
    }

    return NULL;
}

/** find a node matches the param with the match method. */
void * find(list head,node_match match,void * param){
	list cur = head->next;
	while(cur != NULL){
		if(match(cur->value,param) == 1){
            return cur->value;
        }
	    cur = cur->next;
	}

	return NULL;
}

/* retrieve data node from list,and remove it */
void * get_node_from_head(list head){
    if(head->next == NULL){
        return NULL;
    }

    list ret = head->next;

    head->next = head->next->next;

    head->size --;

    void * data = ret->value;
    free_list(ret);

    return data;
}
/* traverse list and display it with print_node function */
void print_list(list head,print_node p){
    list cur = head->next;

    while(cur != NULL){
        p(cur->value);
        cur = cur->next;
    }

    printf("\n");
}

void free_list(list lst){
    free(lst);
}
