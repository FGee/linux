#include "tcp_init.h"

int reuseAddr(int sockFd)
{
	int reuse = 1;
	int ret;
	ret = setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int));
	ERROR_CHECK(ret, -1, "socket");
	
	return 0;
}

int tcpInit(int *psockFd, char *ip, char *port)
{
	int sockFd;
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	ERROR_CHECK(sockFd, -1, "socket");

	struct sockaddr_in ser;
	bzero(&ser, sizeof(struct sockaddr_in));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(port));
	ser.sin_addr.s_addr = inet_addr(ip);

	int ret;
	ret = reuseAddr(sockFd);
	ERROR_CHECK(sockFd, -1, "reuseAddr");

	ret = bind(sockFd, (struct sockaddr*)&ser, sizeof(struct sockaddr_in));
	ERROR_CHECK(ret, -1, "bind");

	ret = listen(sockFd, MAX_CLIENT);
	ERROR_CHECK(ret, -1, "bind");

	*psockFd = sockFd;

	return 0;
}
						
