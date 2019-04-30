#include "function.h"

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

    off_t posOff = 0;//record the position offline of read
    char *pMap = (char*)mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    ERROR_CHECK(pMap, (char*)-1, "mmap");

    train.data_len = sizeof(train.buf);
    while (posOff + (off_t)sizeof(train.buf) <= statbuf.st_size) {
        memcpy(train.buf, pMap + posOff, sizeof(train.buf));
        ret = send(newFd, &train, 4 + train.data_len, 0);
        ERROR_CHECK(ret, -1, "send");
        posOff += sizeof(train.buf);
        //usleep(1);
    }
    //if the file size is shorter than bufUnit and the final remain size, it need:
    off_t finalOff = statbuf.st_size - posOff;
    memcpy(train.buf, pMap + posOff, finalOff);
    train.data_len = finalOff;
    ret = send(newFd, &train, 4 + train.data_len, 0);


#if DEBUG
        printf("send ret -1, kernel do not sinal break\n");
#endif
    train.data_len = 0;
    ret = send(newFd, &train, 4, 0);//the signal of the translaton end 
    ERROR_CHECK(ret, -1, "send");

    return 0;
}
