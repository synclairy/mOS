#include "lib.h"

void *producer(void *args)
{
	sem_t *empty = (sem_t *) ((u_int *) args)[0];
	sem_t *full = (sem_t *) ((u_int *) args)[1];
	int r;
	int i;
	for (i = 0; i < 3; i++) {
		if ((r = sem_trywait(empty)) < 0) {
			writef
			    ("failed to wait, but I can continue producing.\n");
			sem_wait(empty);
			writef("get an empty!\n");
			sem_post(full);
		} else {
			writef("success to wait.\n");
			sem_post(full);
		}
		writef("End producing here.\n");
		syscall_yield();
	}
	sem_destroy(empty);
	sem_destroy(full);
	writef("sem destroyed.");
}

void *consumer1(void *args)
{
	sem_t *empty = (sem_t *) ((u_int *) args)[0];
	sem_t *full = (sem_t *) ((u_int *) args)[1];
	sem_wait(full);
	writef("1:consume full");
	sem_post(empty);
	writef("1:release empty");
}

void *consumer2(void *args)
{
	sem_t *empty = (sem_t *) ((u_int *) args)[0];
	sem_t *full = (sem_t *) ((u_int *) args)[1];
	int r;
	while (1) {
		if ((r = (sem_wait(full))) < 0) {
			break;
		}
		writef("2:consume full");
		if ((r = (sem_wait(empty))) < 0) {
			break;
		}
		writef("2:release empty");
		syscall_yield();
	}
}

void umain()
{
	u_int a[2];
	a[0] = syscall_getthreadid();
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	sem_t empty;
	sem_t full;
	sem_init(&empty, 0, 3);
	sem_init(&full, 0, 0);
	a[0] = &empty;
	a[1] = &full;
	int r;
	pthread_create(&thread2, NULL, consumer1, (void *)a);
	pthread_create(&thread3, NULL, consumer2, (void *)a);
	pthread_create(&thread1, NULL, producer, (void *)a);
	writef("All created!\n");
}
