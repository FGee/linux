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
	ret = bind(sock_fd, (struct sockaddr*)&ser_addr, sizeof(struct sockaddr));
	ERROR_CHECK(ret, -1, "bind");

	ret = listen(sock_fd, 10);
	ERROR_CHECK(ret, -1, "listen");

	struct sockaddr_in cli_addr;
	bzero(&cli_addr, sizeof(cli_addr));
	int new_fd;
	int addrlen = sizeof(struct sockaddr_in);
	new_fd = accept(sock_fd, (struct sockaddr*)&cli_addr, &addrlen);
	ERROR_CHECK(new_fd, -1, "accept");

	printf("client ip = %s, port = %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

	fd_set fdrd;
	struct timeval tv;

	char buf[20] = {0};

	while (1)
	{
		tv.tv_sec = 3;
		FD_ZERO(&fdrd);
		FD_SET(STDIN_FILENO, &fdrd);
		FD_SET(new_fd, &fdrd);

		int ret;
		ret = select(new_fd + 1, &fdrd, NULL, NULL, &tv);
		if (ret > 0)
		{

			if (FD_ISSET(new_fd, &fdrd))
			{
				bzero(buf, sizeof(buf));
				ret = recv(new_fd, buf, sizeof(buf), 0);
				ERROR_CHECK(ret, -1, "recv");
				if (0 == ret)
				{
					printf("bye bye.\n");
					break;
				}
				printf("i am server: %s", buf);
			}

			if (FD_ISSET(STDIN_FILENO, &fdrd))
			{
				memset(buf, 0, sizeof(buf));
				ret = read(STDIN_FILENO, buf, sizeof(buf));
				if (0 == ret)
				{
					printf("bye bye.\n");
					break;
				}
				ret = send(new_fd, buf, strlen(buf), 0);
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

	ret = close(new_fd);
	ERROR_CHECK(ret, -1, "new_fd_close");

	return 0;
}



