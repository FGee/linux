#define _GNU_SOURCE
#include <func.h>
#define BUF_SZ 1000
#define SLICE_SIZE 4096

int recvCycle(int sockFd, char *buf, int dataLenth); //avoid the error when the speed of recv
// is not suitable to the send
