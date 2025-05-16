#ifndef queue_h
#define queue_h

#include <stdlib.h>
#include <stdio.h>

struct wedis_list{
    int size;

    void * value;

    struct wedis_list * next;
    struct wedis_list * prev;
};

typedef struct wedis_list * list;
typedef struct wedis_list * list_node;
typedef int (* node_match)(const void * param1,const void * param2);
typedef void (* print_node)(void * node);

list build_list();
list_node build_list_node(void * value);
void add_node(list lst,void * value);
void * find_from_list(list head,node_match match,void * param);
void * find(list head,node_match match,void * param);
void * get_node_from_head(list head);
void print_list(list head,print_node p);
void free_list(list list);

#endif
