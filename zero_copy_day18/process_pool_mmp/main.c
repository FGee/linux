#include "function.h"

int exitPiFds[2];

//void signalHandle(int sigNum)
void signalHandle(int sigNum, siginfo_t *pInfo, void *pReserved)
{
    write(exitPiFds[1], &sigNum, 1); //only the signal to make epoll listen
}

int main(int argc, char ** argv)
{
    while (fork()) { // when parent exit unnormal, grandparent process create a new
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("parent process exit normal\n");
            exit(0);
        } 
    }
	ARGS_CHECK(argc, 4);
    pipe(exitPiFds);
    //signal(SIGUSR1, signalHandle);
    struct sigaction act;
    bzero(&act, sizeof(struct sigaction));
    act.sa_sigaction = signalHandle;
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act, NULL);

	int ret;
	Process_Data *pChild;
	int childNum = atoi(argv[3]);
	pChild = (Process_Data*)calloc(childNum, sizeof(Process_Data) * childNum);
	ret = makeChild(pChild, childNum);
	ERROR_CHECK(ret, -1, "makeChild");

	int sockFd;
	ret = tcpInit(&sockFd, argv[1], argv[2]);
	ERROR_CHECK(ret, -1, "tcpInit");

	//epoll the file describer
	int epfd;
	epfd = epoll_create(1);
	ERROR_CHECK(epfd, -1, "epoll_create");

	struct epoll_event event, *evs;
	bzero(&event, sizeof(struct epoll_event));
	evs = (struct epoll_event*)calloc(childNum + 1 + 1, sizeof(struct epoll_event));
	event.events = 	EPOLLIN;
	event.data.fd = sockFd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockFd, &event);
	ERROR_CHECK(ret, -1, "epoll_ctl");

	for (int i = 0; i < childNum; ++i) {
		event.data.fd = pChild[i].fd;
		ret = epoll_ctl(epfd, EPOLL_CTL_ADD, pChild[i].fd, &event);
		ERROR_CHECK(ret, -1, "epoll_ctl");
	}

	event.data.fd = exitPiFds[0];//add the event that signalHandle pipe
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, exitPiFds[0], &event);
	ERROR_CHECK(ret, -1, "epoll_ctl");

	int readyFdNum;
	int newFd;
	while (1) {
		readyFdNum = epoll_wait(epfd, evs, childNum + 1, -1);
		//ERROR_CHECK(readyFdNum, -1, "epoll_wait");//must delete this, because when the SIGUSR1 coming, epoll will 
        //return -1, it will make parent process return -1 and make the child become orphan 
        //make the child can recvmsg and continue to run
		for (int i = 0; i < readyFdNum; ++i) {
			if (EPOLLIN == evs[i].events && sockFd == evs[i].data.fd) {
				newFd = accept(sockFd, NULL, NULL);// not need to save 
				//the new struct sockaddr_in to recv and send 
		        ERROR_CHECK(newFd, -1, "accept");
#if DEBUG
                printf("there is accept, newFd %d\n", newFd);
#endif
				for (int j = 0; j < childNum; ++j) {
					if (0 == pChild[j].busy) {
						ret = sendFd(pChild[j].fd, newFd, 0);
						ERROR_CHECK(ret, -1, "sendFd");
						printf("child %d is busy\n", pChild[j].pid);
						pChild[j].busy = 1;
						break;
					}
				}
				close(newFd);//newFd refer count -1; if not exist nonbusy child process, 
				//the link between server and client will termnal
			}
			for (int j = 0; j < childNum; ++j) {
				if (EPOLLIN == evs[i].events && pChild[j].fd == evs[i].data.fd) {
					char Sig_Done; //child finish the task and signal to parent
					ret = read(pChild[j].fd, &Sig_Done, sizeof(char));
					ERROR_CHECK(ret, -1, "read");
					printf("child %d is not busy\n", pChild[j].pid);
					pChild[j].busy = 0;
				}
			}
            if (EPOLLIN == evs[i].events && exitPiFds[0] == evs[i].data.fd) {
                //bootread(exitPiFds[0], &ret, 1);
                printf("start exit\n");
                close(sockFd);
                for (int j = 0; j < childNum; ++j) {
                //    kill(pChild[j].pid, 9);
					ret = sendFd(pChild[j].fd, 0, 1);
					ERROR_CHECK(ret, -1, "sendFd");
#if DEBUG
                    printf("child %d be killed\n", j);
#endif
                }
                for (int j = 0; j < childNum; ++j) {
                    wait(NULL);
                }
                //sleep(8);
                printf("exit success\n");
                return 0;
            }
            
        }
    }
}
