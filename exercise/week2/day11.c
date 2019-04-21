1.
#include <func.h>

void *threadFunc(void *pArg)
{
	int *pival = (int*)pArg;
	printf("i am child thread, the value from main thread is %d\n",
		*pival);
	pthread_exit((void*)2);
	//return (void*)3;
}

int main()
{
	pthread_t thread;
	int ret;
	int num = 1;
	ret = pthread_create(&thread, NULL, threadFunc, (void*)&num);
	THREAD_ERROR_CHECK(ret, "pthread_create");
	//sleep(3);
	int *preturnval;
	ret = pthread_join(thread, (void**)&preturnval);
	printf("the return value of child thread is %d\n", (int)preturnval);

	return 0;
}



2.
#include <func.h>

void cleanFunc(void *p)
{
	char *ps = (char*)p;
	free(ps);
	printf("i am clean func\n");
}

void *threadFunc(void *pArg)
{
	char *ps;
	ps = (char*)calloc(0, 20);
	pthread_cleanup_push(cleanFunc, (void*)ps);
	int *pival = (int*)pArg;
	printf("i am child thread, the value from main thread is %d\n",
		*pival);
	//pthread_exit((void*)2);
	//return (void*)3;
	read(0, ps, sizeof(ps));
	printf("i am clean in the threadFunc\n");
	pthread_cleanup_pop(0);
}

int main()
{
	pthread_t thread;
	int ret;
	int num = 1;
	ret = pthread_create(&thread, NULL, threadFunc, (void*)&num);
	THREAD_ERROR_CHECK(ret, "pthread_create");
	//sleep(3);
	pthread_cancel(thread);
	int *preturnval;
	ret = pthread_join(thread, (void**)&preturnval);
	printf("the return value of child thread is %d\n", (int)preturnval);

	return 0;
}




3.
#include <func.h>
#define N 2
#define M 20000000

typedef struct {
	int num;
	pthread_mutex_t lock;
} Str;

void* thFunc1(void *args)
{
	Str *pargs = (Str*)args;
	int cnt = 0;
	for (int i = 0; i < M; ++i) {
		pthread_mutex_lock(&pargs->lock);
		++pargs->num;
		++cnt;
		pthread_mutex_unlock(&pargs->lock);
	}
	printf("thFunc1 add : %d\n", cnt);
	pthread_exit((void*)1);
}

void* thFunc2(void *args)
{
	Str *pargs = (Str*)args;
	int cnt = 0;
	for (int i = 0; i < M; ++i) {
		pthread_mutex_lock(&pargs->lock);
		++pargs->num;
		++cnt;
		pthread_mutex_unlock(&pargs->lock);
	}
	printf("thFunc2 add : %d\n", cnt);
	pthread_exit((void*)2);
}

typedef void* (*pthFunc)(void *args);

int main()
{
	pthread_t thArr[2];
	pthFunc pthfunc[2] = { thFunc1, thFunc2 };
	Str args;
	bzero((void*)&args, sizeof(Str));
	int ret;

	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_TIMED_NP);
	pthread_mutex_init(&args.lock, &mutexAttr);
	for (int i = 0; i < N; ++i) {
		ret = pthread_create(thArr + i, NULL, pthfunc[i], (void*)&args);
		THREAD_ERROR_CHECK(ret, "pthread_create");
	}

	int returnval;
	for (int i = 0; i < N; ++i) {
		pthread_join(thArr[i], (void**)&returnval);
		printf("thFunc%d is joined\n", returnval);
	}

	printf("the anwser is %d after add\n", args.num);

	pthread_mutex_destroy(&args.lock);

	return 0;
}



4.
#include <func.h>
#define N 3
#define INIT_TICKET 20
#define TICKET_ADD 20
#define T 500000

typedef struct {
	int num;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	pthread_t thArr[N - 1];
} Str;

void cleanFunc(void *args)
{
	Str *pargs = (Str*)args;
	pthread_mutex_unlock(&pargs->lock);
}

void* thFunc1(void *args)
{
	Str *pargs = (Str*)args;
	int cnt = 0;
	while (1) {
		pthread_mutex_lock(&pargs->lock);
		pthread_cleanup_push(cleanFunc, (void*)pargs);
		if (pargs->num > 0) {
			printf("ticket remain : %d , th1 sold %d\n", pargs->num, cnt);
			--pargs->num;
			if (0 == pargs->num) {
				printf("th1 will signal\n");
				pthread_cond_signal(&pargs->cond);
				printf("th1 had signal\n");
			}
			++cnt;
			printf("ticket remain : %d , th1 sold %d\n", pargs->num, cnt);
			pthread_mutex_unlock(&pargs->lock);
		}
		else {
			pthread_mutex_unlock(&pargs->lock);
		}
		pthread_cleanup_pop(0);
		usleep(T);
	}
	pthread_exit((void*)1);
}

void* thFunc2(void *args)
{
	Str *pargs = (Str*)args;
	int cnt = 0;
	while (1) {
		pthread_mutex_lock(&pargs->lock);
		pthread_cleanup_push(cleanFunc, (void*)pargs);
		if (pargs->num > 0) {
			printf("ticket remain : %d , th2 sold %d\n", pargs->num, cnt);
			--pargs->num;
			if (0 == pargs->num) {
				printf("th1 will signal\n");
				pthread_cond_signal(&pargs->cond);
				printf("th1 had signal\n");
			}
			++cnt;
			printf("ticket remain : %d , th2 sold %d\n", pargs->num, cnt);
			pthread_mutex_unlock(&pargs->lock);
		}
		else {
			pthread_mutex_unlock(&pargs->lock);
		}
		pthread_cleanup_pop(0);
		usleep(T);
	}
	pthread_exit((void*)2);
}

void* th_add_func(void *args)
{
	Str *pargs = (Str*)args;
	pthread_mutex_lock(&pargs->lock);
	printf("cond sucess\n");
	pthread_cond_wait(&pargs->cond, &pargs->lock);
	pargs->num = TICKET_ADD;
	printf("add sucess\n");
	pthread_mutex_unlock(&pargs->lock);
	while (1) {
		if (0 == pargs->num) {
			for (int i = 0; i < N - 1; ++i) {
				pthread_cancel(pargs->thArr[i]);
				printf("th%d cancel\n", i + 1);
			}
			break;
		}
	}
	pthread_exit((void*)3);
}

typedef void* (*pthFunc)(void *args);

int main()
{
	pthread_t thArr[N];
	pthFunc pthfunc[N] = { thFunc1, thFunc2, th_add_func };
	Str args;
	bzero((void*)&args, sizeof(Str));
	int ret;

	args.num = INIT_TICKET;

	pthread_cond_init(&args.cond, NULL);

	pthread_mutex_init(&args.lock, NULL);
	for (int i = 0; i < N; ++i) {
		ret = pthread_create(thArr + i, NULL, pthfunc[i], (void*)&args);
		printf("th%d id %ld\n", i + 1, thArr[i]);
		if (i < N - 1) {
			args.thArr[i] = thArr[i];
		}
		THREAD_ERROR_CHECK(ret, "pthread_create");
	}

	int returnval;
	for (int i = 0; i < N; ++i) {
		pthread_join(thArr[i], (void**)&returnval);
		printf("thFunc%d is joined\n", returnval);
	}

	pthread_mutex_destroy(&args.lock);

	return 0;
}



