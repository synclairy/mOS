#include "lib.h"

void *test(void *arg)
{
	while (1) {
		writef("son: I'm still alive\n");
		syscall_yield();
	}
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
	pthread_cancel(thread);
	writef("son is canceled by father\n");
}
