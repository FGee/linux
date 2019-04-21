#include <func.h>
#define MSG_QUE_KEY 8000
#define SZ 1<<8

typedef struct {
	long mytype;
	char mtext[SZ];
} Msg_msq;

typedef struct {
	int msqid;	
} ID;

ID id;

void signhander(int iSignNo) //clear work
{
	msgctl(id.msqid, IPC_RMID, NULL);
	exit(0);
}

int main()
{
	//sign to kill myself and shmdel
	signal(SIGINT, signhander);

	//create message queue to comunicate with ouput window two
	id.msqid = msgget(MSG_QUE_KEY, IPC_CREAT | 0600);
	Msg_msq msg;

	while (1)
	{
		bzero((void*)&msg, sizeof(Msg_msq));
		msgrcv(id.msqid, &msg, sizeof(Msg_msq), 0, 0);
		if (1024 == msg.mytype) {
			printf("%s", msg.mtext);
			kill(getpid(), SIGINT);
		} else if (1 == msg.mytype) {
			printf("Mike: %s", msg.mtext);
		} else if (2 == msg.mytype) {
			printf("I: %s", msg.mtext);
		}
	}
}
