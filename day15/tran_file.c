#include "function.h"

int tranFile(int newFd)
{
    Train train;
    train.data_len = strlen(FILENAME);
    strcpy(train.buf, FILENAME);
    int ret;
    ret = send(newFd, &train, 4 + train.data_len, 0);
    ERROR_CHECK(ret, -1, "send");

    int fd = open(FILENAME, O_RDONLY);
    while ((train.data_len = read(fd, train.buf, sizeof(train.buf)))) {
        ret = send(newFd, &train, 4 + train.data_len, 0);
        ERROR_CHECK(ret, -1, "send");
        //usleep(1);
    }
    ret = send(newFd, &train, 4, 0);//the signal of the translaton end 
    ERROR_CHECK(ret, -1, "send");

    return 0;
}
