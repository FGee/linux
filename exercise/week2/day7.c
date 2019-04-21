1.
#include <func.h>

int main()
{
	printf("pid = %d, ppid = %d, uid = %d, gid = %d, euid = %d, egid = %d", getpid(), getppid(), getuid(), getgid(), geteuid(), getegid());

	return 0;
}
2.
#include <func.h>

int main()
{
	pid_t iRet;
	iRet = fork();
	if (0 == iRet) {
		printf("I am child process, pid = %d, ppid = %d\n", getpid(), getppid());
		while (1);
	}
	else {
		printf("I am parent process, pid = %d, ppid = %d\n", getpid(), getppid());
		while (1);
	}

	return 0;
}






3.
//文件的不同进程的dup
#include <func.h>

int main(int argc, char ** argv)
{
	ARGS_CHECK(argc, 2);
	int fd;
	fd = open(argv[1], O_RDWR);
	ERROR_CHECK(fd, -1, "open");
	pid_t iRet;
	iRet = fork();
	if (0 == iRet)
	{
		printf("i am child process. pid = %d, ppid = %d", getpid(), getppid());
		write(fd, " world", 6);
	}
	else {
		printf("i am parent process. pid = %d, ppid = %d", getpid(), getppid());
		write(fd, "hello", 5);
		sleep(5);
	}
	return 0;
}


//栈空间的写时复制
#include <func.h>

int main()
{
	char *buf;
	buf = (char*)calloc(20, sizeof(char));
	strcpy(buf, "hello world");
	pid_t iRet;
	iRet = fork();
	if (0 == iRet)
	{
		printf("i am child process. pid = %d, ppid = %d\n", getpid(), getppid());
		write(1, buf, strlen(buf));
		putc('\n', stdout);
	}
	else {
		printf("i am parent process. pid = %d, ppid = %d\n", getpid(), getppid());
		fwrite(buf, sizeof(char), strlen(buf) + 2, stdout);
		fputc('\n', stdout);
		strcpy(buf, "how are you");
		puts(buf);
	}
	return 0;
}



4.
#include <func.h>

int main(int argc, char ** argv)
{
	ARGS_CHECK(argc, 3);
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	printf("%d + %d = %d\n", a, b, a + b);

	return 0;
}

#include <func.h>

int main(int argc, char ** argv)
{
	ARGS_CHECK(argc, 5);
	execl(argv[1], argv[2], argv[3], argv[4], NULL);
	printf("i am execl");

	return 0;
}



