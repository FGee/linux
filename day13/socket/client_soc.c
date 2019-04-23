#include <func.h>

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
	
	ret = send(sock_fd, "hello", strlen("hello"), 0);
	ERROR_CHECK(ret, -1, "send");

	char buf[20] = {0};
	ret = recv(sock_fd, buf, sizeof(buf), 0);
	ERROR_CHECK(ret, -1, "recv");
	printf("i am client, gets %s\n", buf);


	ret = close(sock_fd);
	ERROR_CHECK(ret, -1, "ser_fd_close");

	return 0;
}
	


