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

	ret = recv(sock_fd, &dataLen, 4, 0);
	ERROR_CHECK(ret, -1, "recv");
	ret = recv(sock_fd, buf, dataLen, 0);
	ERROR_CHECK(ret, -1, "recv");
	buf[dataLen] = '\0'; //must to add the void symbol to end the string
	int fd = open(buf, O_CREAT | O_WRONLY, 0666);
	ERROR_CHECK(fd, -1, "open");
	
	while (1) {
		ret = recv(sock_fd, &dataLen, 4, 0);
		ERROR_CHECK(ret, -1, "recv");
		if (dataLen > 0) {
			ret = recv(sock_fd, buf, dataLen, 0);
			ERROR_CHECK(ret, -1, "recv");
			//printf("1111\n");
			ret = write(fd, buf, dataLen);
			ERROR_CHECK(ret, -1, "write");
			//buf[dataLen] = '\0';
			//printf("%s ret %d\n", buf, ret);
		} else {
			//printf("break\n");
			break;
		}
	}
	
	//sync();
	close(fd);
	printf("Finish download\n");
	close(sock_fd);

	return 0;
}
