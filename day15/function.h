#include <func.h>
#define MAX_CLIENT 10
#define TRAIN_SZ 1000
#define FILENAME "file"

typedef struct {
	pid_t pid; // child process pid
	int fd; // the one edge of pipe between parent process and child
	short busy; // child busy 1, or sleep 0
} Process_Data;

int makeChild(Process_Data* pChild, int childNum); //create the child process by childNum

int childHandle(int pipeFd); //the task child process to finish

int tcpInit(int *psockFd, char *ip, char *port); //initial the tcp to make it listening

int sendFd(int pipe_fd, int newFd);//send the newFd to connect with client,
//from parent to child

int recvFd(int pipe_fd, int *pNewFd);//receve the newFd from 
//parent to child

typedef struct {
	int data_len; //contol the translate
	char buf[TRAIN_SZ]; //capture data
} Train;

int tranFile(int newFd); //the task of child

int reuseAddr(int sockFd); //reuse address , must before bind
