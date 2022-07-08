#include "lib.h"
pthread_t tt[4] = { 0 };

void *simple(void *args)
{
	int i;
	writef("simple's stack around %x\n", &i);
	for (i = 0; i < 4; i++) {
		writef("simple%d\n", i);
		syscall_yield();
	}
	writef("simple ends\n");
}

void *stackk(void *args)
{
	int i;
	writef("stackk's stack around %x\n", &i);
	writef("string is %s\n", (char *)args);
}

void *complex1(void *args)
{
	int i;
	writef("complex's stack around %x\n", &i);
	int *a = (int *)args;
	u_int *ret;
	char stack_test[100] = { 'S', 'T', 'A', 'C', 'K' };
	pthread_join(*(a), ret);
	for (i = 0; i < 4; i++) {
		writef("complex%d\n", i);
		syscall_yield();
	}
	pthread_create(a + 2, NULL, stackk, (void *)stack_test);
	writef("complex ends\n");
}

void umain()
{
	pthread_t a[4] = { 0 };
/*
	pthread_create(a, NULL, test0, (void *)a);
	pthread_create(a + 1, NULL, test1, (void *)a);
	pthread_create(a + 2, NULL, test2, (void *)a);
	pthread_create(a + 3, NULL, test3, (void *)a);
*/
	writef("main's stack around %x\n", a);
	pthread_create(a, NULL, simple, (void *)a);
	pthread_create(a + 1, NULL, complex1, (void *)a);
	writef("all created!\n");
	u_int *ret;
	pthread_join(a[0], ret);
	pthread_join(a[1], ret);
	writef("Main thread over.\n");
}
