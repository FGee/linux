#include "work_que.h"

int queInit(pQue_t pq, int cap)
{
    bzero(pq, sizeof(Que_t));
    pthread_mutex_init(&pq->que_mutex, NULL);
    pq->capacity = cap;

    return 0;
}

int queInsert(pQue_t pq, pNode_t pnew)
{
    if (NULL == pq->ptail) {
        pq->ptail = pnew;
        pq->phead = pnew;
    } else {
        pq->ptail->pNext = pnew;
        pq->ptail = pnew;
    }
    ++pq->size;

    return 0;
}

int queGet(pQue_t pq, pNode_t *pget)
{
    if (pq->size > 0) {//avoid the special situation
        *pget = pq->phead;
        if (NULL == pq->phead->pNext) {
            pq->phead = NULL;
            pq->ptail = NULL;
        } else {
            pq->phead = pq->phead->pNext;
        }
        --pq->size;
        return 1;
    } else {
        return 0;
    }
}

