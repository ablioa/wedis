#include "queue.h"

TaskPool * pool;

/**
 *  build task model
 */ 
Task * buildTask(int taskType){
    Task * task = (Task *) malloc(sizeof(Task));
    memset(task,0,sizeof(Task));

    task->taskType = taskType;
    task->next = NULL;

    return task;
}

/**
 * initialize task pool
 */
void initTaskPool(){
    pool = (TaskPool *) calloc(1,sizeof(TaskPool));

    pool->head = NULL;
    pool->tail = NULL;
    pool->size = 0;
}

/**
 * add task to task pool
 */
void addTask(TaskPool * pool,Task * task){
    if(pool->tail == NULL){
        pool->tail = task;
        pool->head = task;
    }else{
        pool->tail->next = task;
        pool->tail = task;
    }

    pool->size ++;
}

/**
 * get task from pool
 */
Task * getTask(TaskPool * pool){
    if(pool->head == NULL){
        return NULL;
    }

    Task * ret = pool->head;

    pool->head = pool->head->next;
    
    if(pool->head == NULL){
        pool->tail = NULL;
    }

    pool->size --;

    return ret;
}
