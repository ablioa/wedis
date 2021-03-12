#ifndef queue_h
#define queue_h

#include <stdlib.h>

struct redis_list{
    int size;

    void * value;

    struct redis_list * next;
    struct redis_list * prev;

    struct redis_list * head;
    struct redis_list * tail;
};

typedef struct redis_list * list;

list init_list(void * value);

void add_node(list lst,void * value);

void * get_node(list lst);

#endif
