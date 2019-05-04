#include "func_cli.h"
#define SLICE_SIZE 4096

int main(int argc, char** argv)
{
    ARGS_CHECK(argc, 3);
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sock_fd, -1, "socket");

    struct sockaddr_in ser_addr;
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(atoi(argv[2]));
    ser_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret;

    ret = connect(sock_fd, (struct sockaddr*)&ser_addr, sizeof(struct sockaddr_in));
    ERROR_CHECK(ret, -1, "conect");

    printf("conect success\n");

    int dataLen;
    char buf[BUF_SZ];

    ret = recvCycle(sock_fd, (char*)&dataLen, 4);
    ERROR_CHECK(ret, -1, "recvCycle");
    ret = recvCycle(sock_fd, buf, dataLen);
    ERROR_CHECK(ret, -1, "recvCycle");
    buf[dataLen] = '\0'; //must to add the void symbol to end the string
    int fd = open(buf, O_CREAT | O_RDWR, 0666);//must can read and write
    ERROR_CHECK(fd, -1, "open");

    off_t file_size = 0; //receve the file size
    ret = recvCycle(sock_fd, (char*)&dataLen, 4);
    ERROR_CHECK(ret, -1, "recvCycle");
    ret = recvCycle(sock_fd, buf, dataLen);
    ERROR_CHECK(ret, -1, "recvCycle");
    file_size = *(off_t*)buf;

#if DEBUG
    printf("file size %ld\n", file_size);
#endif

    struct timeval start, end;
    gettimeofday(&start, NULL);

    off_t recvSize = 0;
    off_t returnSize = 0;
    off_t slice_size = (off_t)dataLen / 10000;
    off_t pre_size = 0;
    int fdps[2];
    pipe(fdps);

    while (recvSize + SLICE_SIZE < file_size) {
        returnSize = splice(sock_fd, NULL, fdps[1], NULL, SLICE_SIZE, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (0 == returnSize) {
            printf("\ndownload fail\n");
            close(fd);
            close(sock_fd);
            return -1;
        }
        ERROR_CHECK(returnSize, -1, "splice1");
        returnSize = splice(fdps[0], NULL, fd, NULL, returnSize, SPLICE_F_MOVE | SPLICE_F_MORE);
        ERROR_CHECK(returnSize, -1, "splice2");
        recvSize += returnSize;
        if (recvSize - pre_size > slice_size) {
            printf("\r%5.2f%%", (float)recvSize / file_size * 100);
            fflush(stdout); //flush the stdout buf, same as '\n'
            pre_size = recvSize;
        }
    }
    printf("\n");
        returnSize = splice(sock_fd, NULL, fdps[1], NULL, file_size - recvSize, SPLICE_F_MOVE | SPLICE_F_MORE);
        ERROR_CHECK(returnSize, -1, "splice");
        returnSize = splice(fdps[0], NULL, fd, NULL, returnSize, SPLICE_F_MOVE | SPLICE_F_MORE);
        ERROR_CHECK(returnSize, -1, "splice");

    gettimeofday(&end, NULL);
    printf("use time %ld us\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

    close(fd);

    ret = recvCycle(sock_fd, (char*)&dataLen, 4); //end of translate
    ERROR_CHECK(ret, -1, "recvCycle");
    ret = recvCycle(sock_fd, buf, dataLen);
    ERROR_CHECK(ret, -1, "recvCycle");
    
    printf("Finish download\n");
    close(sock_fd);

    return 0;
}
