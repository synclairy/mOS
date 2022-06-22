#include "lib.h"

void umain()
{
	volatile u_int *a = (volatile u_int *)0x23333334;
	writef("a = %x\n", *a);
	make_shared((void *)a);
	writef("a = %x\n", *a);
	*a = 233;
	writef("a = %x\n", *a);
	if (fork() == 0) {
		u_int ch = syscall_getenvid();
		*a = ch;
		writef("fork = 0,a = %x\n", *a);
		while (*a == ch) {
			syscall_yield();
		}
		writef("parent is %x\n", *a);
	} else {
		writef("fork != 0,a = %x\n", *a);
		while (*a == 233) {
			syscall_yield();
		}
		writef("child is %x\n", *a);
		*a = syscall_getenvid();
	}
}
