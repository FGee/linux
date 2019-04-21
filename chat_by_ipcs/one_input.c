#include <func.h>
#define SHM_LOCK_KEY 1000
#define SEM_LOCK_KEY 2000
#define SHM_MSG_KEY  4000
#define MSG_BUF_SZ 1<<8
#define BUF_SZ 1<<8

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
	semctl(id.semid_lock, 0, IPC_RMID);
	shmctl(id.shmid_msg, IPC_RMID, NULL);
	exit(0);
}

int main(int argc, char** argv)
{
	ARGS_CHECK(argc, 3);

	//sign to kill myself and shmdel
	signal(SIGINT, signhander);

	//read and write lock
	id.semid_lock = semget(SEM_LOCK_KEY, 1, IPC_CREAT | 0600);
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
	pMymsg = (Msg*)shmat(id.shmid_msg, NULL, 0); 

	//conect pipe for comunicate between one and two 
	int fd_pipe_read, fd_pipe_write;
	fd_pipe_read = open(argv[1], O_RDONLY);
	ERROR_CHECK(fd_pipe_read, -1, "open1");
	fd_pipe_write = open(argv[2], O_WRONLY);
	ERROR_CHECK(fd_pipe_write, -1, "open2");
	fd_set fdrd;

	struct timeval tv;

	char buf[BUF_SZ] = {0};

	while (1)
	{
		tv.tv_sec = 1<<10;
		FD_ZERO(&fdrd);
		FD_SET(0, &fdrd);
		FD_SET(fd_pipe_read, &fdrd);

		int ret;
		ret = select(fd_pipe_read + 1, &fdrd, NULL, NULL, &tv);

		if (ret > 0)
		{
			memset(buf, 0, sizeof(buf));
			if (FD_ISSET(fd_pipe_read, &fdrd))
			{
				ret = read(fd_pipe_read, buf, sizeof(buf));
				if (0 == ret)//while the pipe, write and read this is not use
				{
					//send term msg to chat window
					semop(id.semid_lock, &opp, 1);
					pMymsg->flag = -1;
					strcpy(pMymsg->msg_buf, "Mary: Bye bye!\n");
					semop(id.semid_lock, &opv, 1);
					//send close signal by close pipe
					close(fd_pipe_read);
					close(fd_pipe_write);
					//disat pMsg form shared memory
					shmdt(pMymsg);
					//send SIGINT to myself
					kill(getpid(), SIGINT);
				}
				semop(id.semid_lock, &opp, 1);
				pMymsg->flag = 2;
				strcpy(pMymsg->msg_buf, buf);
				semop(id.semid_lock, &opv, 1);
			}

			memset(buf, 0, sizeof(buf));
			if (FD_ISSET(0, &fdrd))
			{
				ret = read(0, buf, sizeof(buf));
				if (buf[0] == 'q' && buf[1] == 't')
				{
					//send term msg to chat window
					semop(id.semid_lock, &opp, 1);
					pMymsg->flag = -1;
					semop(id.semid_lock, &opv, 1);
					//send close signal by close pipe
					close(fd_pipe_read);
					close(fd_pipe_write);
					//disat pMsg form shared memory
					shmdt(pMymsg);
					//send SIGINT to myself
					kill(getpid(), SIGINT);
				}
				//send msg to one chat window
				semop(id.semid_lock, &opp, 1);
				pMymsg->flag = 1;
				strcpy(pMymsg->msg_buf, buf);
				semop(id.semid_lock, &opv, 1);
				//send msg to two
				write(fd_pipe_write, buf, strlen(buf));
			}
		}
		else if (0 == ret)
		{
			printf("time out\n");
		}
	}
}
