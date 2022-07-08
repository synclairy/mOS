#include "lib.h"

void *producer(void *args)
{
	sem_t *empty = (sem_t *) ((u_int *) args)[0];
	sem_t *full = (sem_t *) ((u_int *) args)[1];
	int r;
	int i;
/*	sem_getvalue(empty, &r);
	writef("%xempty is %d\n",empty, r);
	sem_getvalue(full, &r);
	writef("%xfull is %d\n",full, r);
	writef("1 at %x,  2 at %x\n", &r, &i);
*/
	for (i = 0; i < 10; i++) {
		sem_wait(empty);
		sem_getvalue(empty, &r);
		writef("producer: empty is %d\n", r);
		sem_post(full);
		sem_getvalue(full, &r);
		writef("producer: full is %d\n", r);
		writef("Producer have produced!\n");
	}
	sem_destroy(empty);
	sem_destroy(full);
	writef("sem destroyedi by Producer.\n");
}

void *consumer1(void *args)
{
	sem_t *empty = (sem_t *) ((u_int *) args)[0];
	sem_t *full = (sem_t *) ((u_int *) args)[1];
	int r;
	while (1) {
		writef("consumer1:it is my turn\n");
		if ((r = (sem_wait(full))) < 0) {
			writef("consumer1: sem destroyed!\n", r);
			break;
		}
		writef("consumer1:consume full\n");
		if ((r = (sem_post(empty))) < 0) {
			writef("consumer1: sem destroyed!\n", r);
			break;
		}
		writef("consumer1:release empty\n");
		writef("consumer1 yield.\n");
		syscall_yield();
	}
}

void *consumer2(void *args)
{
	sem_t *empty = (sem_t *) ((u_int *) args)[0];
	sem_t *full = (sem_t *) ((u_int *) args)[1];
	int r;
	while (1) {
		if ((r = (sem_wait(full))) < 0) {
			writef("consumer1:it is my turn\n");
			writef("consumer2: sem destroyed!\n", r);
			break;
		}
		writef("consumer2:consume full\n");
		if ((r = (sem_wait(full))) < 0) {
			writef("consumer2: sem destroyed!\n", r);
			break;
		}
		writef("consumer2:consume full\n");
		if ((r = (sem_post(empty))) < 0) {
			writef("consumer2: sem destroyed!\n", r);
			break;
		}
		writef("consumer2:release empty\n");
		if ((r = (sem_post(empty))) < 0) {
			writef("consumer2: sem destroyed!\n", r);
			break;
		}
		writef("consumer2:release empty\n");
		writef("consumer2 yield.\n");
		syscall_yield();
	}
}

void umain()
{
	u_int a[2];
	sem_t full1;
	sem_init(&full1, 0, 1);
	sem_t empty1;
	sem_init(&empty1, 0, 1);
	a[0] = &empty1;
	a[1] = &full1;
//      writef("empty at %x, full at %x\n", &empty1, &full1);
	int r;
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	sem_getvalue(&empty1, &r);
	writef("main: empty is %d\n", r);
	sem_getvalue(&full1, &r);
	writef("main: full is %d\n", r);
	pthread_create(&thread2, NULL, consumer1, (void *)a);
	pthread_create(&thread3, NULL, consumer2, (void *)a);
	pthread_create(&thread1, NULL, producer, (void *)a);
	writef("All created!\n");
}
