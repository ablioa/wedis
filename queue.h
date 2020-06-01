#ifndef queue_h
#define queue_h

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

struct task{
    int taskType;
    struct task * next;
};

struct taskpool{
    int size;
    struct task * head;
    struct task * tail;
};

typedef struct task Task;
typedef struct taskpool TaskPool;

extern TaskPool * pool;

Task * buildTask(int taskType);

void initTaskPool();

void addTask(TaskPool * pool,Task * task);

Task * getTask(TaskPool * pool);

#endif
