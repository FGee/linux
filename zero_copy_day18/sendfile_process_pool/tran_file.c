#include "function.h"

int sendCycle(int newFd, char *pMap, int lenth)
{
    int total = 0;
    int ret;
    while (total < lenth) {
        ret = send(newFd, pMap + total, lenth - total, 0);
        ERROR_CHECK(ret, -1, "send");//important: client disconect while translating , the surver continu to work
        total += ret;
    }
    return 0;
}

int tranFile(int newFd)
{
    Train train;
    train.data_len = strlen(FILENAME);
    strcpy(train.buf, FILENAME);
    int ret;
#if DEBUG
    printf("kill -10 bug : here is send 10 in tranFile\n");
#endif
    ret = send(newFd, &train, 4 + train.data_len, 0);
    ERROR_CHECK(ret, -1, "send");

    int fd = open(FILENAME, O_RDONLY);

    struct stat statbuf; //acknowladge the file size information
    bzero(&statbuf, sizeof(struct stat));
    ret = fstat(fd, &statbuf);
    ERROR_CHECK(ret, -1, "fstat");
    train.data_len = sizeof(statbuf.st_size);
    memcpy(train.buf, &statbuf.st_size, sizeof(statbuf.st_size));
    ret = send(newFd, &train, 4 + train.data_len, 0);
    ERROR_CHECK(ret, -1, "send");

#if DEBUG // verify the information that put into buf
    off_t st_size = *(off_t*)train.buf;
    printf("the file size is %ld\n", st_size);
#endif

    ret = sendfile(newFd, fd, NULL, statbuf.st_size);
    printf("sendfile return value %d\n", ret);

#if DEBUG
        printf("send ret -1, kernel do not sinal break\n");
#endif
    train.data_len = 0; //the signal of end
    ret = send(newFd, &train, 4 + train.data_len, 0);
    ERROR_CHECK(ret, -1, "send");

    return 0;
}
