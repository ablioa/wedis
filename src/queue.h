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

typedef int (* node_match)(void * param1,void * param2);

typedef void (* print_node)(void * node);

list build_list();

void add_node(list lst,void * value);

void * get_node(list lst);

void * find_from_list(list head,node_match match,void * param);

/** find a node matches the param with the match method. */
void * find(list head,node_match match,void * param);

void free_list(list list);

void print_list(list head,print_node p);
#endif
