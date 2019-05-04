#include "factory.h"

void* threadTask(void *pArg)
{
    //int ret;
    pFactory_t pf = (pFactory_t)pArg;
    pQue_t pq = &pf->work_que;
    pNode_t pget = NULL;
    int getSuccess = 0;
    while (1) {
        if (pf->startFlag || pq->size) {
            pthread_mutex_lock(&pq->que_mutex);
            pthread_cond_wait(&pf->cond, &pq->que_mutex);
            getSuccess = queGet(pq, &pget);
            pthread_mutex_unlock(&pq->que_mutex);
            if (getSuccess) {
                tranFile(pget->fd);
                close(pget->fd);
                free(pget);
                getSuccess = 0;
            }
        } else {
            pthread_exit(NULL);
        }
    }
}

int factoryInit(pFactory_t pf, int thrNum, int cap)
{
    int ret;
    bzero(pf, sizeof(Factory_t));
    pf->threadNum = thrNum;
    pf->pthid = (pthread_t*)calloc(thrNum, sizeof(pthread_t));
    ERROR_CHECK(pf->pthid, NULL, "calloc");//heat space not enough

    ret = queInit(&pf->work_que, cap);
    ERROR_CHECK(ret, -1, "queInit");

    ret = pthread_cond_init(&pf->cond, NULL);

    return 0;
}

int factoryStart(pFactory_t pf)
{
    //int ret;
    for (int i = 0; i < pf->threadNum; ++i) {
        pthread_create(pf->pthid + i, NULL, threadTask, (void*)pf);
    }
    pf->startFlag = 1;

    return 0;
}

int factoryDestroy(pFactory_t pf)
{
    for (int i = 0; i < pf->threadNum; ++i) {//avoid deadlock at the condition args
        pthread_cond_signal(&pf->cond);
    }
    for (int i = 0; i < pf->threadNum; ++i) {
        pthread_join(pf->pthid[i], NULL);
    }
    pthread_mutex_destroy(&pf->work_que.que_mutex);
    pthread_cond_destroy(&pf->cond);

    return 0;
}

