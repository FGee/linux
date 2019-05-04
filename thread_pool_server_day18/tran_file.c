#include "tran_file.h"

int sendCycle(int newfd, char *pMap, int lenth)
{
    int total = 0;
    int ret;
    while (total < lenth) {
        ret = send(newfd, pMap + total, lenth - total, 0);
        ERROR_CHECK(ret, -1, "send");//important: client disconect while translating , the surver continu to work
        total += ret;
    }
    return 0;
}

int tranFile(int newfd)
{
    Train train;
    train.data_len = strlen(FILENAME);
    strcpy(train.buf, FILENAME);
    int ret;
    ret = send(newfd, &train, 4 + train.data_len, 0);
    ERROR_CHECK(ret, -1, "send");

    int fd = open(FILENAME, O_RDONLY);

    struct stat statbuf; //acknowladge the file size information
    bzero(&statbuf, sizeof(struct stat));
    ret = fstat(fd, &statbuf);
    ERROR_CHECK(ret, -1, "fstat");
    train.data_len = sizeof(statbuf.st_size);
    memcpy(train.buf, &statbuf.st_size, sizeof(statbuf.st_size));
    ret = send(newfd, &train, 4 + train.data_len, 0);
    ERROR_CHECK(ret, -1, "send");

    ret = sendfile(newfd, fd, NULL, statbuf.st_size);
    ERROR_CHECK(ret, -1, "sendfile");
    printf("sendfile return value %d\n", ret);

    //important: client disconect while translating , the surver continu to work
    //if not have this , send will break the child process
    if (ret < statbuf.st_size) {
        return -1;
    }

    train.data_len = 0; //the signal of end
    ret = send(newfd, &train, 4 + train.data_len, 0);
    ERROR_CHECK(ret, -1, "send");

    return 0;
}
