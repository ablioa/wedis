#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

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
    pool = (TaskPool *) malloc(sizeof(TaskPool));
    memset(pool,0,sizeof(TaskPool));

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

    printf("task pool size: %d \n",pool->size);
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
    pool->size --;

	printf("task pool size: %d \n",pool->size);

    return ret;
}


// int main(){

//     initTaskPool();

//     for(int ix = 0; ix < 5; ix ++){
//         Task * task = buildTask(ix*ix+4);
//         addTask(pool,task);

// 		printf("real-size: %d\n",pool->size);
//     }

// 	printf("final-size: %d\n",pool->size);

//     for(int ix = 0; ix < 10; ix ++){
//         Task * task = getTask(pool);

// 		if(task == NULL){
// 		    printf("EMPTY POOL!!!\n");
// 			continue;
// 		}

// 		printf("real-size: %d : taskType: %d\n",pool->size,task->taskType);
//     }

// 	printf("final-size: %d\n",pool->size);

//     return 0;
// }
