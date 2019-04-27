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
    int fd = open(buf, O_CREAT | O_WRONLY, 0666);
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
    
    time_t past, now;
    off_t total_size = 0;
    past = now = time(NULL);

    while (1) {
        ret = recvCycle(sock_fd, (char*)&dataLen, 4);
        ERROR_CHECK(ret, -1, "recvCycle");
        if (dataLen > 0) {
            ret = recvCycle(sock_fd, buf, dataLen);
            ERROR_CHECK(ret, -1, "recvCycle");
            ret = write(fd, buf, dataLen);
            ERROR_CHECK(ret, -1, "write");

            total_size += ret;

            now = time(NULL);
            if (now - past > 1) {
                printf("\r%5.2f%%", (float)total_size / file_size * 100);
                fflush(stdout); //flush the stdout buf, same as '\n'
                past = now;
            }
        } else {
            printf("\r100.00%%\n");
            break;
        }
    }

    close(fd);
    printf("Finish download\n");
    close(sock_fd);

    return 0;
}
