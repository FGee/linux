#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#include "tran_file.h"

typedef struct {
    pthread_t *pthid;//store the start pointer point to thread id
    int threadNum;//the thread number which will to create
    pthread_cond_t cond;//the condition arugement that sync between main thread and child thread
    Que_t work_que;//store the task that main thread produce
    short startFlag;// the signal of factory start
} Factory_t, *pFactory_t;

int factoryInit(pFactory_t pfactory, int, int);

int factoryStart(pFactory_t);

int factoryDestroy(pFactory_t pf);
#endif

