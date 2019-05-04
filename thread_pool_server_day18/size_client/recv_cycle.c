#include "func_cli.h"

int recvCycle(int sock_fd, char *buf, int dataLen)
{
    off_t slice_size = (off_t)dataLen / 10000;
    off_t pre_size = 0;
    off_t total_recv = 0;
    int ret;
    while (total_recv < dataLen) {
        ret = recv(sock_fd, buf + total_recv, dataLen - total_recv, 0);
        if (0 == ret) {
            printf("\nserver update, download fail\n");
            exit(-1);
        }
        ERROR_CHECK(ret, -1, "recv");
        total_recv += ret;

        if (total_recv - pre_size > slice_size) {
            printf("\r%5.2f%%", (float)total_recv / dataLen * 100);
            fflush(stdout); //flush the stdout buf, same as '\n'
            pre_size = total_recv;

        }
    }
    printf("\n");

    return 0;
}

