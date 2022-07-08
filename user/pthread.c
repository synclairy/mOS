#include "lib.h"
#include <error.h>
#include <mmu.h>

int pthread_create(pthread_t * thread, const pthread_attr_t * attr,
		   void *(*start_rountine)(void *), void *arg)
{
	int newthread;
	if ((newthread = syscall_thread_alloc()) < 0) {
		thread = 0;
		return newthread;
	}
	struct Thread *t = &env->env_threads[newthread];
	t->thread_tf.regs[4] = arg;
	t->thread_tf.regs[31] = exit;
	t->thread_tf.regs[29] -= 4;
	t->thread_tf.pc = start_rountine;
	syscall_set_thread_status(t->thread_id, ENV_RUNNABLE);
	*thread = t->thread_id;
	return 0;
}

int pthread_setcanceltype(int type, int *oldtype)
{
	u_int threadid = syscall_getthreadid();
	struct Thread *t = &env->env_threads[THREADX(threadid)];
	if ((type != THREAD_CANCEL_ENABLE) & (type != THREAD_CANCEL_DISABLE)) {
		return -1;
	}
	if (t->thread_id != threadid) {
		return -1;
	}
	*oldtype = t->thread_canceltype;
	t->thread_canceltype = type;
	return 0;
}

int pthread_detach(pthread_t thread)
{
	return 0;
}

void pthread_exit(void *retval)
{
	u_int threadid = syscall_getthreadid();
	struct Thread *t = &env->env_threads[THREADX(threadid)];
	t->thread_exit_ptr = retval;
	syscall_thread_destroy(threadid);
}

int pthread_cancel(pthread_t thread)
{
	struct Thread *t = &env->env_threads[THREADX(thread)];
	if ((t->thread_id != thread) | (t->thread_status == ENV_FREE)) {
		return -E_THREAD_NOT_FOUND;
	}
	if (t->thread_canceltype == THREAD_CANCEL_DISABLE) {
		return -E_THREAD_NOT_CANCELABLE;
	}
	t->thread_exit_value = -1;
	syscall_thread_destroy(thread);
	return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
	int r = syscall_thread_join(thread, retval);
	return r;
}
