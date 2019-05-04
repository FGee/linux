#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"

typedef struct node {
    int fd;
    struct node *pNext;
} Node_t, *pNode_t;

typedef struct {
    pNode_t phead;
    pNode_t ptail;
    pthread_mutex_t que_mutex;//the mutex of que
    int size;
    int capacity;
} Que_t, *pQue_t;


int queInit(pQue_t pq, int cap);
int queInsert(pQue_t pq, pNode_t pnew);
int queGet(pQue_t pq, pNode_t *pget);

#endif
