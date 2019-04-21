#include <func.h>
#define MSG_QUE_KEY 8000
#define MTEXT_SZ 1<<8
#define BUF_SZ 1<<8

typedef struct {
	long mytype;
	char mtext[MTEXT_SZ];
} Msg_msq;

typedef struct {
	int msqid;	
} ID;

ID id;

void signhander(int iSignNo) //clear work
{
	//sleep(3);//wait the chat window
	msgctl(id.msqid, IPC_RMID, NULL);
	exit(0);
}

int main(int argc, char** argv)
{
	ARGS_CHECK(argc, 3);

	//sign to kill myself and shmdel
	signal(SIGINT, signhander);

	//create message queue to comunicate with ouput window two
	id.msqid = msgget(MSG_QUE_KEY, IPC_CREAT | 0600);
	Msg_msq msg;
	bzero((Msg_msq*)&msg, sizeof(Msg_msq));

	//conect pipe for comunicate between one and two 
	int fd_pipe_write, fd_pipe_read;
	fd_pipe_write = open(argv[1], O_WRONLY);
	ERROR_CHECK(fd_pipe_write, -1, "open1");
	fd_pipe_read = open(argv[2], O_RDONLY);
	ERROR_CHECK(fd_pipe_read, -1, "open2");
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
			bzero((void*)&msg, sizeof(Msg_msq));
			memset(buf, 0, sizeof(buf));
			if (FD_ISSET(fd_pipe_read, &fdrd))
			{
				ret = read(fd_pipe_read, buf, sizeof(buf));
				if (0 == ret)
				{
					//send term msg to chat window
					msg.mytype = 1024;
					strcpy(msg.mtext, "Mike: Bye bye!\n");
					msgsnd(id.msqid, &msg, sizeof(Msg_msq), 0);
					//send close signal by close pipe
					close(fd_pipe_write);
					close(fd_pipe_read);
					//send SIGINT to myself
					kill(getpid(), SIGINT);
				}
				msg.mytype = 1;
				memcpy((void*)&msg.mtext, buf, sizeof(buf));
				ret = msgsnd(id.msqid, &msg, sizeof(Msg_msq), 0);
			}

			bzero((void*)&msg, sizeof(Msg_msq));
			memset(buf, 0, sizeof(buf));
			if (FD_ISSET(0, &fdrd))
			{
				ret = read(0, buf, sizeof(buf));
				if (buf[0] == 'q' && buf[1] == 't')
				{
					//send term msg to chat window
					msg.mytype = 1024;
					msgsnd(id.msqid, &msg, sizeof(Msg_msq), 0);
					//send close signal by close pipe
					close(fd_pipe_write);
					close(fd_pipe_read);
					//send SIGINT to myself
					kill(getpid(), SIGINT);
				}
				//send msg to one chat window
				strcpy(msg.mtext, buf);
				msg.mytype = 2;
				msgsnd(id.msqid, &msg, sizeof(Msg_msq), 0);
				//send msg to two
				ret = write(fd_pipe_write, buf, strlen(buf));
			}
		}
		else if (0 == ret)
		{
			printf("time out\n");
		}
	}
}
