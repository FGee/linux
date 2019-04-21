1.
#include <func.h>

int main()
{
	int fds[2];
	int ret;
	ret = pipe(fds);
	ERROR_CHECK(ret, -1, "pipe");

	if (!fork()) {
		printf("i am child. pid = %d, ppid = %d\n", getpid(), getppid());
		char buf[20];
		read(0, buf, sizeof(buf) - 1);
		close(fds[0]);
		write(fds[1], buf, strlen(buf));
	}
	else {
		printf("i am parent. pid = %d, ppid = %d\n", getpid(), getppid());
		char buf[20] = { 0 };
		close(fds[1]);
		read(fds[0], buf, sizeof(buf));
		write(1, buf, sizeof(buf));
		sleep(5);
	}
	return 0;
}








2.
#include <func.h>

int main()
{
	const char *path = "/home/shane/homework/day2/shm";
	key_t key;
	key = ftok(path, 'a');
	printf("the key is %d\n", key);
	int shmid;
	shmid = shmget(key, 4 * (1 << 10), IPC_CREAT | 0666);
	ERROR_CHECK(shmid, -1, "shmget");
	printf("the shmid is %d\n", shmid);
	char *p;
	p = (char*)shmat(shmid, NULL, 0);
	ERROR_CHECK(p, (char*)-1, "shmat");

	strcpy(p, "How are you\n");

	return 0;
}


#include <func.h>

int main()
{
	const char *path = "/home/shane/homework/day2/shm";
	key_t key;
	key = ftok(path, 'a');
	printf("the key is %d\n", key);
	int shmid;
	shmid = shmget(key, 4 * (1 << 10), IPC_CREAT);
	ERROR_CHECK(shmid, -1, "shmget");
	printf("the shmid is %d\n", shmid);
	char *p;
	p = (char*)shmat(shmid, NULL, 0);
	ERROR_CHECK(p, (char*)-1, "shmat");

	puts(p);
	shmdt(p);
	int ret;
	ret = shmctl(shmid, IPC_RMID, NULL);
	printf("shmctl return %d\n", ret);

	return 0;
}








3.
#include <func.h>
#define N 10000000

int main()
{
	int shmid;
	shmid = shmget(10000, 4 * (1 << 10), IPC_CREAT | 0666);
	ERROR_CHECK(shmid, -1, "shmget");
	printf("the shmid is %d\n", shmid);
	int *p;
	p = (int*)shmat(shmid, NULL, 0);
	ERROR_CHECK(p, (int*)-1, "shmat");
	memset(p, 0, sizeof(int));
	if (!fork()) {
		for (int i = 0; i < N; ++i) {
			++p[0];
		}
		return 0;
	}
	else {
		for (int i = 0; i < N; ++i)
			++p[0];
		wait(NULL);
		printf("%d\n", *p);
		return 0;
	}
}





