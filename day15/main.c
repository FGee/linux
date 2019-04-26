#include "function.h"

int main(int argc, char ** argv)
{
	int ret;
	ARGS_CHECK(argc, 4);
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
	evs = (struct epoll_event*)calloc(childNum + 1, sizeof(struct epoll_event));
	event.events = 	EPOLLIN;
	event.data.fd = sockFd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockFd, &event);
	ERROR_CHECK(ret, -1, "epoll_ctl");

	for (int i = 0; i < childNum; ++i) {
		event.data.fd = pChild[i].fd;
		ret = epoll_ctl(epfd, EPOLL_CTL_ADD, pChild[i].fd, &event);
		ERROR_CHECK(ret, -1, "epoll_ctl");
	}
	
	int readyFdNum;
	int newFd;
	while (1) {
		readyFdNum = epoll_wait(epfd, evs, childNum + 1, -1);
		ERROR_CHECK(readyFdNum, -1, "epoll_wait");
		for (int i = 0; i < readyFdNum; ++i) {
			if (EPOLLIN == evs[i].events && sockFd == evs[i].data.fd) {
				newFd = accept(sockFd, NULL, NULL);// not need to save 
				//the new struct sockaddr_in to recv and send 
				for (int j = 0; j < childNum; ++j) {
					if (0 == pChild[j].busy) {
						ret = sendFd(pChild[j].fd, newFd);
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
		}
	}
}
