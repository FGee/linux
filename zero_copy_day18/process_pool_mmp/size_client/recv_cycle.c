#include "func_cli.h"

int recvCycle(int sockFd, char *buf, int dataLenth)
{
    int total_recv = 0;
    int ret;
    while (total_recv < dataLenth) {
        ret = recv(sockFd, buf + total_recv, dataLenth - total_recv, 0);
        if (0 == ret) {
            printf("\nserver update, download fail\n");
            exit(-1);
        }
        ERROR_CHECK(ret, -1, "recv");
        total_recv += ret;
    }

    return 0;
}

