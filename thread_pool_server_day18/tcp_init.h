#ifndef __TCP_INIT_H__
#define __TCP_INIT_H__
#include "head.h"

#define MAX_CLIENT 20

int tcpInit(int *psockFd, char *ip, char *port);
#endif 
