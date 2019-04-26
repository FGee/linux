#include "function.h"

int makeChild(Process_Data *pChild, int childNum)
{
	int pid;
	int fds[2];
	int ret;
	for (int i = 0; i < childNum; ++i) {
		ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
		ERROR_CHECK(ret, -1, "socketpair");
		pid = fork();
		if (!pid) {
			close(fds[1]);
			childHandle(fds[0]);
		}
		pChild[i].pid = pid;
		close(fds[0]);
		pChild[i].fd = fds[1];
		pChild[i].busy = 0;
	}
	return 0;
}

int childHandle(int pipeFd)
{
	int ret;
	int newFd;
	while (1) {
		ret = recvFd(pipeFd, &newFd);
		ERROR_CHECK(ret, -1, "recvFd");
		
		tranFile(newFd);

		close(newFd);

		char Sig_Done = 'D'; 
		ret = write(pipeFd, &Sig_Done, sizeof(char));
	}
}
