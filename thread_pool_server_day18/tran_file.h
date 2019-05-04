#ifndef __TRAN_FILE_H__
#define __TRAN_FILE_H__
#include "head.h"

#define FILENAME "file"
#define TRAIN_SZ 1000

typedef struct {
	int data_len; //contol the translate
	char buf[TRAIN_SZ]; //capture data
} Train;

int sendCycle(int newfd, char *pMap, int lenth);
int tranFile(int newfd);

#endif
