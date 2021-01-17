#include "queue.h"

list init_list(void * value){
    list lst = (list)calloc(1,sizeof(struct redis_list));
    lst->value = value;
    return lst;
}

void free_list(list lst){
    free(lst);
}

void add_node(list lst,void * value){
    list data = init_list(value);

    if(lst->tail == NULL){
        lst->tail = data;
        lst->head = data;
    }else{
        lst->tail->next = data;
        lst->tail = data;
    }

    lst->size ++;

    ///////////////////////////////////////
}

void * get_node(list lst){
    if(lst->head == NULL){
        return NULL;
    }

    list ret = lst->head;

    lst->head = lst->head->next;
    
    if(lst->head == NULL){
        lst->tail = NULL;
    }

    lst->size --;
    
    void * data = ret->value;
    free_list(ret);

    return data;
}
