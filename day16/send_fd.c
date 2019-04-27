#include "function.h"
#define CHECK_SIZE 10

int sendFd(int pipe_fd, int newFd, int exitFlag)
{
	struct msghdr msg;
	bzero(&msg, sizeof(struct msghdr));

	//char buf1[CHECK_SIZE] = "hello";
	char buf2[CHECK_SIZE] = "world";
	struct iovec iov[2];
	iov[0].iov_base = &exitFlag; //transfer the exitFlag from parent to child
	iov[0].iov_len = sizeof(int);
	iov[1].iov_base = buf2;
	iov[1].iov_len = 5;
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	
	struct cmsghdr *pCmsg;
	int len = CMSG_LEN(sizeof(int));
	pCmsg = (struct cmsghdr*)calloc(1, len);
	bzero(pCmsg, sizeof(struct cmsghdr));
	pCmsg->cmsg_len = len;
	pCmsg->cmsg_level = SOL_SOCKET;
	pCmsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(pCmsg) = newFd; // push the fd which be trans 
	// to cmsghdr
	msg.msg_control = pCmsg;
	msg.msg_controllen = len;
	int ret;
	ret = sendmsg(pipe_fd, &msg, 0);
	ERROR_CHECK(ret, -1, "sendmsg");

	return 0;
}

int recvFd(int pipe_fd, int *pNewFd, int *pExitFlag)
{
	struct msghdr msg;
	bzero(&msg, sizeof(struct msghdr));

	//char buf1[CHECK_SIZE] = { 0 };
	char buf2[CHECK_SIZE] = { 0 };
	struct iovec iov[2];
	iov[0].iov_base = pExitFlag;
	iov[0].iov_len = sizeof(int);
	iov[1].iov_base = buf2;
	iov[1].iov_len = 5;
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	
	struct cmsghdr *pCmsg;
	int len = CMSG_LEN(sizeof(int));
	pCmsg = (struct cmsghdr*)calloc(1, len);
	bzero(pCmsg, sizeof(struct cmsghdr));
	pCmsg->cmsg_len = len;
	pCmsg->cmsg_level = SOL_SOCKET;
	pCmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control = pCmsg;
	msg.msg_controllen = len;
	int ret;
	ret = recvmsg(pipe_fd, &msg, 0);
	ERROR_CHECK(ret, -1, "recvmsg");
	*pNewFd = *(int*)CMSG_DATA(pCmsg); // copy and distribute the fd which be trans from parent, is similar as dup2 

	return 0;
}

