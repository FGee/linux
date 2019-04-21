#include <func.h>
#define SHM_LOCK_KEY 1000
#define SEM_LOCK_KEY 2000
#define SHM_MSG_KEY  4000
#define MSG_BUF_SZ 1<<8

typedef struct {                                                
	int flag;
	char msg_buf[MSG_BUF_SZ];
} Msg, *pMsg;

typedef struct {
	int semid_lock;
	int shmid_msg;
} ID;

ID id;

void signhander(int iSignNo) //clear work
{
	shmdt(&id.shmid_msg);
	exit(0);
}

int main()
{
	//sign to kill myself and shmdel
	signal(SIGINT, signhander);

	//read and write lock
	id.semid_lock = semget(SEM_LOCK_KEY, 1, IPC_CREAT | 0600);
	ERROR_CHECK(id.semid_lock, -1, "semget_lock");
	semctl(id.semid_lock, 0, SETVAL, 1);

	struct sembuf opp, opv;
	opp.sem_num = 0;
	opp.sem_op = -1;
	opp.sem_flg = SEM_UNDO;
	opv.sem_num = 0;
	opv.sem_op = 1;
	opv.sem_flg = SEM_UNDO;

	//creat shared memory for message
	pMsg pMymsg;

	id.shmid_msg = shmget(SHM_MSG_KEY, sizeof(Msg), IPC_CREAT | 0600);
	ERROR_CHECK(id.shmid_msg, -1, "semget_msg");
	pMymsg = (Msg*)shmat(id.shmid_msg, NULL, 0); 

	while (1) {
		semop(id.semid_lock, &opp, 1);
		if (-1 == pMymsg->flag) {
			printf("%s", pMymsg->msg_buf);
			//disat the p and pMsg form shared memory
			shmdt(pMymsg);
			kill(getpid(), SIGINT);
		} else if (1 == pMymsg->flag) {
			printf("I: %s", pMymsg->msg_buf);
			bzero((void*)pMymsg, sizeof(Msg));
		} else if (2 == pMymsg->flag) {
			printf("Mary: %s", pMymsg->msg_buf);
			bzero((void*)pMymsg, sizeof(Msg));
		}
		semop(id.semid_lock, &opv, 1);
	}
}
