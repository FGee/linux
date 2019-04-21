1.
#include <func.h>

void sighander(int iSignNo)
{
	printf("%d is coming\n", iSignNo);
	sleep(4);
	printf("%d is going\n", iSignNo);
}

int main()
{
	while (signal(SIGINT, sighander) == SIG_ERR) {
		perror("signal");
		return -1;
	}
	signal(SIGQUIT, sighander);
	char buf[20];
	//while (1) {
	bzero(buf, sizeof(buf));
	read(STDIN_FILENO, buf, sizeof(buf) - 1);
	fputs(buf, stdout);
	//}

	return 0;
}




2.
#include <func.h>

void sighandernew(int iSignNo, siginfo_t *pInfo, void *pReserved)
{
	printf("%d is coming. send process pid %d uid %d\n", iSignNo, pInfo->si_pid, pInfo->si_uid);
	sleep(5);
	sigset_t pend;
	sigpending(&pend);
	if (sigismember(&pend, SIGQUIT)) {
		puts("yes\n");
	}
	else {
		puts("no\n");
	}
	printf("%d is leaving.\n", iSignNo);
}

int main()
{
	struct sigaction act;
	act.sa_handler = NULL;
	act.sa_sigaction = sighandernew;
	sigemptyset(&act.sa_mask);

	act.sa_flags = SA_SIGINFO;
	act.sa_restorer = NULL;
	sigfillset(&act.sa_mask);
	if (sigismember(&act.sa_mask, SIGQUIT))
	{
		puts("yes\n");
	}
	else {
		puts("no");
	}

	sigaction(SIGINT, &act, NULL);

	while (1);

	return 0;
}




3.
#include <func.h>

void sighandernew(int iSignNo, siginfo_t *pInfo, void *pReserved)
{
	printf("%d is coming. send process pid %d uid %d\n", iSignNo, pInfo->si_pid, pInfo->si_uid);
	alarm(5);
	pause();
	sigset_t pend;
	sigpending(&pend);
	if (sigismember(&pend, SIGQUIT)) {
		puts("yes\n");
	}
	else {
		puts("no\n");
	}
	printf("%d is leaving.\n", iSignNo);
}

int main()
{
	struct sigaction act;
	act.sa_handler = NULL;
	act.sa_sigaction = sighandernew;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGQUIT);

	act.sa_flags = SA_SIGINFO;
	act.sa_restorer = NULL;

	sigaction(SIGINT, &act, NULL);

	while (1);

	return 0;
}


4.
#include <func.h>

void signhander(int iSignNo)
{
	printf("%d is coming\n", iSignNo);
	printf("%d is leaving\n", iSignNo);
}

int main()
{
	signal(SIGINT, signhander);
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	sleep(5);
	sigset_t pend;
	sigpending(&pend);
	if (sigismember(&pend, SIGINT)) {
		puts("SIGINT is pending\n");
	}
	else {
		puts("SIGINT is not pending\n");
	}
	sigprocmask(SIG_UNBLOCK, &mask, NULL);

	while (1);

	return 0;
}



5.
//真实计时器
#include <func.h>

void signhander(int iSignNo)
{
	printf("%d is coming\n", iSignNo);
	time_t now;
	char *ptime;
	time(&now);
	ptime = ctime(&now);
	printf("%s\n", ptime);
	printf("%d is leaving\n", iSignNo);
}

int main()
{
	signal(SIGALRM, signhander);
	struct itimerval value;
	bzero(&value, sizeof(value));
	value.it_interval.tv_sec = 2;
	value.it_value.tv_sec = 4;
	signhander(0);
	setitimer(ITIMER_REAL, &value, NULL);
	sleep(20);
	while (1);

	return 0;
}




//虚拟计时器
#include <func.h>

void signhander(int iSignNo)
{
	printf("%d is coming\n", iSignNo);
	time_t now;
	char *ptime;
	time(&now);
	ptime = ctime(&now);
	printf("%s\n", ptime);
	printf("%d is leaving\n", iSignNo);
}

int main()
{
	signal(SIGVTALRM, signhander);
	struct itimerval value;
	bzero(&value, sizeof(value));
	value.it_interval.tv_sec = 2;
	value.it_value.tv_sec = 4;
	signhander(0);
	setitimer(ITIMER_VIRTUAL, &value, NULL);
	sleep(5);
	while (1);

	return 0;
}



//实用计时器
#include <func.h>

void signhander(int iSignNo)
{
	printf("%d is coming\n", iSignNo);
	time_t now;
	char *ptime;
	time(&now);
	ptime = ctime(&now);
	printf("%s\n", ptime);
	printf("%d is leaving\n", iSignNo);
}

int main()
{
	signal(SIGPROF, signhander);
	struct itimerval value;
	bzero(&value, sizeof(value));
	value.it_interval.tv_sec = 2;
	value.it_value.tv_sec = 4;
	signhander(0);
	setitimer(ITIMER_PROF, &value, NULL);
	sleep(5);
	while (1);

	return 0;
}


