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

	fd_set fdrd;
	struct timeval tv;

	char buf[20] = {0};

	while (1)
	{
		tv.tv_sec = 5;
		FD_ZERO(&fdrd);
		FD_SET(0, &fdrd);
		FD_SET(sock_fd, &fdrd);

		int ret;
		ret = select(sock_fd + 1, &fdrd, NULL, NULL, &tv);

		if (ret > 0)
		{
			if (FD_ISSET(sock_fd, &fdrd))
			{
				memset(buf, 0, sizeof(buf));
				ret = recv(sock_fd, buf, sizeof(buf), 0);
				ERROR_CHECK(ret, -1, "recv");
				if (0 == ret)
				{
					printf("bye bye.\n");
					break;
				}
				printf("i am client: %s", buf);
			}

			if (FD_ISSET(0, &fdrd))
			{
				memset(buf, 0, sizeof(buf));
				ret = read(0, buf, sizeof(buf));
				if (0 == ret)
				{
					printf("bye bye.\n");
					break;
				}
				ret = send(sock_fd, buf, strlen(buf), 0);
				ERROR_CHECK(ret, -1, "send");
			}
		}
		else if (0 == ret)
		{
			printf("time out\n");
		}
	}

	ret = close(sock_fd);
	ERROR_CHECK(ret, -1, "ser_fd_close");

	return 0;
}
