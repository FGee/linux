1.
#include <func.h>
#define MAXFILE 64

void Daemon()
{
	if (fork()) { //creat child , parent exit
		exit(0);
	}
	setsid();
	chdir("/");
	umask(0);
	for (int i = 0; i < MAXFILE; i++) {
		close(i);
	}
}

int main()
{
	Daemon();
	FILE *fp;

	while (1) {
		fp = fopen("/tmp/log", "ab");
		fwrite("I have run 5s\n", sizeof(char), strlen("I have run 5s\n"), fp);
		fclose(fp);
		sleep(5);
	}
}







2.
#include <func.h>
#define N 10000000

int main()
{
	int shmid;
	shmid = shmget(1000, 4, IPC_CREAT | 0600);
	int *p;
	p = (int*)shmat(shmid, NULL, 0);
	p[0] = 0;

	int semid;
	semid = semget(1000, 1, IPC_CREAT | 0600);
	semctl(semid, 0, SETVAL, 1);

	struct sembuf opp, opv;
	opp.sem_num = 0;
	opp.sem_op = -1;
	opp.sem_flg = SEM_UNDO;
	opv.sem_num = 0;
	opv.sem_op = 1;
	opv.sem_flg = SEM_UNDO;

	if (!fork()) {
		for (int i = 0; i < N; ++i) {
			semop(semid, &opp, 1);
			++p[0];
			semop(semid, &opv, 1);
		}
		return 0;
	}
	else {
		for (int i = 0; i < N; ++i) {
			semop(semid, &opp, 1);
			++p[0];
			semop(semid, &opv, 1);
		}
		wait(NULL);
		printf("The anwser is %d.\n", p[0]);
		semctl(semid, 0, IPC_RMID);
		shmctl(shmid, IPC_RMID, 0);
		return 0;
	}
}



3.
//20张票
#include <func.h>
#define N 10000000

int main()
{
	int shmid;
	shmid = shmget(1000, 4, IPC_CREAT | 0600);
	int *p;
	p = (int*)shmat(shmid, NULL, 0);
	p[0] = 20;

	int semid;
	semid = semget(1000, 1, IPC_CREAT | 0600);
	semctl(semid, 0, SETVAL, 1);

	struct sembuf opp, opv;
	opp.sem_num = 0;
	opp.sem_op = -1;
	opp.sem_flg = SEM_UNDO;
	opv.sem_num = 0;
	opv.sem_op = 1;
	opv.sem_flg = SEM_UNDO;
	int cnt = 0;

	if (!fork()) {
		while (1) {
			if (0 == p[0]) {
				printf("child sold %d\n", cnt);
				break;
			}
			semop(semid, &opp, 1);
			--p[0];
			semop(semid, &opv, 1);
			++cnt;
		}
		return 0;
	}
	else {
		while (1) {
			if (0 == p[0]) {
				printf("parent sold %d\n", cnt);
				break;
			}
			semop(semid, &opp, 1);
			--p[0];
			semop(semid, &opv, 1);
			++cnt;
		}
		wait(NULL);
		semctl(semid, 0, IPC_RMID);
		shmctl(shmid, IPC_RMID, 0);
		return 0;
	}
}
//两千万张票
#include <func.h>

int main()
{
	int shmid;
	shmid = shmget(1000, 4, IPC_CREAT | 0600);
	int *p;
	p = (int*)shmat(shmid, NULL, 0);
	p[0] = 20000000;

	int semid;
	semid = semget(1000, 1, IPC_CREAT | 0600);
	semctl(semid, 0, SETVAL, 1);

	struct sembuf opp, opv;
	opp.sem_num = 0;
	opp.sem_op = -1;
	opp.sem_flg = SEM_UNDO;
	opv.sem_num = 0;
	opv.sem_op = 1;
	opv.sem_flg = SEM_UNDO;
	int cnt = 0;

	if (!fork()) {
		while (1) {
			if (0 == p[0]) {
				printf("child sold %d\n", cnt);
				break;
			}
			semop(semid, &opp, 1);
			--p[0];
			semop(semid, &opv, 1);
			++cnt;
		}
		return 0;
	}
	else {
		while (1) {
			if (0 == p[0]) {
				printf("parent sold %d\n", cnt);
				break;
			}
			semop(semid, &opp, 1);
			--p[0];
			semop(semid, &opv, 1);
			++cnt;
		}
		wait(NULL);
		semctl(semid, 0, IPC_RMID);
		shmctl(shmid, IPC_RMID, 0);
		return 0;
	}
}



由上可知，所得公平性体现在并发行上，当多个进程的并发性很大时，越公平。

4.
#include <func.h>

int main()
{
	int semidArr;
	semidArr = semget(1000, 2, IPC_CREAT | 0600);
	short arr[2] = { 10, 0 };
	semctl(semidArr, 0, SETALL, arr);
	struct sembuf sopp, sopv;


	if (!fork()) {
		sopp.sem_num = 0;
		sopp.sem_op = -1;
		sopp.sem_flg = SEM_UNDO;
		sopv.sem_num = 1;
		sopv.sem_op = 1;
		sopv.sem_flg = SEM_UNDO;

		while (1) {
			printf("i am producer. productor : %d , spaces : %d\n", semctl(semidArr, 1, GETVAL), semctl(semidArr, 0, GETVAL));
			semop(semidArr, &sopp, 1);
			printf("i will product\n");
			semop(semidArr, &sopv, 1);
			printf("i am producer. productor : %d , spaces : %d\n", semctl(semidArr, 1, GETVAL), semctl(semidArr, 0, GETVAL));

			sleep(4);
		}
	}
	else {
		sopp.sem_num = 1;
		sopp.sem_op = -1;
		sopp.sem_flg = SEM_UNDO;
		sopv.sem_num = 0;
		sopv.sem_op = 1;
		sopv.sem_flg = SEM_UNDO;
		sleep(3);
		while (1) {
			printf("i am consumer. productor : %d , spaces : %d\n", semctl(semidArr, 1, GETVAL), semctl(semidArr, 0, GETVAL));
			semop(semidArr, &sopp, 1);
			printf("i will consume\n");
			semop(semidArr, &sopv, 1);
			printf("i am consumer. productor : %d , spaces : %d\n", semctl(semidArr, 1, GETVAL), semctl(semidArr, 0, GETVAL));
			sleep(8);
		}
	}
}


