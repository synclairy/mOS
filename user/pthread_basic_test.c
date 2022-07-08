#include "lib.h"
void *print_array(void *arg)
{
	int arg0 = ((int *)arg)[0];
	int arg1 = ((int *)arg)[1];
	int arg2 = ((int *)arg)[2];
	int *temp1 = (int *)(((int *)arg)[3]);
	int *temp2 = (int *)(((int *)arg)[4]);
	writef("Son:arg 0 is %d\n", arg0);
	writef("Son:arg 1 is %d\n", arg1);
	writef("Son:arg 2 is %d\n", arg2);
	++(*temp2);
	writef("Son:b is changed!\n");
	while ((*temp1) == 1) {
		syscall_yield();
		//writef("Son:Dad, I'm still waiting for you.\n");
	}
	writef("Son:I got it, a is %d\n.", (*temp1));
}

void umain()
{
	int a = 1;
	int b = 0;
	int thread;
	int array[5];
	array[0] = 1;
	array[1] = 22;
	array[2] = 333;
	array[3] = &a;
	array[4] = &b;
	pthread_t son;
	thread = pthread_create(&son, NULL, print_array, (void *)array);
	writef("Father:Son, I gave you an array. Can you print it out?\n");
	while (b == 0) {
		syscall_yield();
		//writef("Father:Son, I'm still waiting for you.\n");
	}
	writef("Father:You are right and a is changed.\n");
	a++;
	syscall_yield();
}
