#include "head.h"
#include "factory.h"
#include "work_que.h"
#include "tcp_init.h"

int fds[2];

void sigHandler(int sigNo)
{
    write(fds[1], &sigNo, sizeof(int));
}

int main(int argc, char ** argv)
{
    ARGS_CHECK(argc, 5);
    int ret;

    pipe(fds);
    if (fork()) {
        close(fds[0]);
        signal(SIGUSR1, sigHandler);

        int status;
        wait(&status);
#ifdef DEBUG
        printf("%d\n", WIFEXITED(status));
        printf("the child proess had been clear\n");
#endif
        exit(0);
    }
    close(fds[1]);

    Factory_t f;
    memset(&f, 0, sizeof(Factory_t));
    int threadNUm = atoi(argv[3]);
    int capicity = atoi(argv[4]);
    ret = factoryInit(&f, threadNUm, capicity);
    ERROR_CHECK(ret, -1, "factoryInit");

    ret = factoryStart(&f);
    ERROR_CHECK(ret, -1, "factoryStart");

    int sockFd;
    ret = tcpInit(&sockFd, argv[1], argv[2]);
    ERROR_CHECK(ret, -1, "tcpInit");

    pQue_t pq = &f.work_que;
    int newfd;

    int epfd;
    epfd = epoll_create(1);

    struct epoll_event evs[2], event;
    bzero(evs, sizeof(struct epoll_event) * 2);
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = sockFd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockFd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    event.data.fd = fds[0];
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fds[0], &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");

    int readyNum;

    while (1) {
        readyNum = epoll_wait(epfd, evs, 2, -1);
        for (int i = 0; i < readyNum; ++i) {
            if(EPOLLIN == evs[i].events && evs[i].data.fd == sockFd) {
                newfd = accept(sockFd, NULL, NULL);
                ERROR_CHECK(ret, -1, "accept");

                if (pq->size <= pq->capacity) {
                    pNode_t pnew = (pNode_t)calloc(1, sizeof(Node_t));
                    ERROR_CHECK(pnew, NULL, "calloc");
                    pnew->fd = newfd;
                    pthread_mutex_lock(&pq->que_mutex);
                    ret = queInsert(pq, pnew);
                    ERROR_CHECK(ret, -1, "queInsert");
                    pthread_mutex_unlock(&pq->que_mutex);

                    pthread_cond_signal(&f.cond);
                } else {
                    close(newfd);
                }
            }
            if(EPOLLIN == evs[i].events && evs[i].data.fd == fds[0]) {
                ret = read(fds[0], &ret, sizeof(int));
                f.startFlag = 0;
#ifdef DEBUG
                printf("the factory will be destroyed\n");
#endif
                factoryDestroy(&f);
#ifdef DEBUG
                printf("the factory had been destroyed\n");
#endif
                return 0;
            }
        }
    }
}

