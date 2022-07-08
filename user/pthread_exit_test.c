#include "lib.h"

void *test(void *arg)
{
	int ret = -111;
	writef("son try to exit with ret = -111!\n");
	pthread_exit(&ret);
	writef("son not exit!\n");
}

void umain()
{
	pthread_t thread;
	int i = 0;
	int a[5] = { 0 };
	pthread_create(&thread, NULL, test, (void *)a);
	writef("son is create!\n");
	for (i = 0; i < 3; i++) {
		syscall_yield();
	}
	writef("son exit ret is %d\n",
	       *((int *)env->env_threads[THREADX(thread)].thread_exit_ptr));
}
