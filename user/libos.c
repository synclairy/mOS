#include "lib.h"
#include <mmu.h>
#include <env.h>

void exit(void)
{
	//close_all();
	//syscall_env_destroy(0);
	struct Thread *t = &env->env_threads[THREADX(syscall_getthreadid())];
	t->thread_exit_value = 0;
	syscall_thread_destroy(0);
}

struct Env *env;
struct Thread *thread;

void libmain(int argc, char **argv)
{
	// set env to point at our env structure in envs[].
	env = 0;		// Your code here.
	thread = 0;
	//writef("xxxxxxxxx %x  %x  xxxxxxxxx\n",argc,(int)argv);
	int envid;
	envid = syscall_getenvid();
	envid = ENVX(envid);
	env = &envs[envid];
	// call user main routine
	int threadno;
	threadno = THREADX(syscall_getthreadid());
	thread = &env->env_threads[threadno];
	umain(argc, argv);
	// exit gracefully
	exit();
	//syscall_env_destroy(0);
}
