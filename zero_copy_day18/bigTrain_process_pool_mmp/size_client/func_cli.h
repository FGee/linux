#include <func.h>
#define BUF_SZ 1000

int recvCycle(int sockFd, char *buf, int dataLenth); //avoid the error when the speed of recv
// is not suitable to the send
