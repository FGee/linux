#include "func_cli.h"

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

    ftruncate(fd, file_size);
    char *pMap = (char*)mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, fd, 0); 

    struct timeval start, end;
    gettimeofday(&start, NULL);

    ret = recvCycle(sock_fd, pMap, file_size);
    ERROR_CHECK(ret, -1, "recvCycle");

    gettimeofday(&end, NULL);
    printf("use time %ld us\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

    munmap(pMap, file_size);
    close(fd);

    ret = recvCycle(sock_fd, (char*)&dataLen, 4); //end of translate
    ERROR_CHECK(ret, -1, "recvCycle");
    ret = recvCycle(sock_fd, buf, dataLen);
    ERROR_CHECK(ret, -1, "recvCycle");
    
    printf("Finish download\n");
    close(sock_fd);

    return 0;
}
