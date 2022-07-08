#include "../drivers/gxconsole/dev_cons.h"
#include <mmu.h>
#include <env.h>
#include <printf.h>
#include <pmap.h>
#include <sched.h>
#include <error.h>

extern char *KERNEL_SP;
extern struct Env *curenv;

/* Overview:
 * 	This function is used to print a character on screen.
 *
 * Pre-Condition:
 * 	`c` is the character you want to print.
 */
void sys_putchar(int sysno, int c, int a2, int a3, int a4, int a5)
{
	printcharc((char)c);
	return;
}

/* Overview:
 * 	This function enables you to copy content of `srcaddr` to `destaddr`.
 *
 * Pre-Condition:
 * 	`destaddr` and `srcaddr` can't be NULL. Also, the `srcaddr` area
 * 	shouldn't overlap the `destaddr`, otherwise the behavior of this
 * 	function is undefined.
 *
 * Post-Condition:
 * 	the content of `destaddr` area(from `destaddr` to `destaddr`+`len`) will
 * be same as that of `srcaddr` area.
 */
void *memcpy(void *destaddr, void const *srcaddr, u_int len)
{
	char *dest = destaddr;
	char const *src = srcaddr;

	while (len-- > 0) {
		*dest++ = *src++;
	}

	return destaddr;
}

/* Overview:
 *	This function provides the environment id of current process.
 *
 * Post-Condition:
 * 	return the current environment id
 */
u_int sys_getenvid(void)
{
	return curenv->env_id;
}

u_int sys_getthreadid(void)
{
	return curthread->thread_id;
}

/* Overview:
 *	This function enables the current process to give up CPU.
 *
 * Post-Condition:
 * 	Deschedule current process. This function will never return.
 *
 * Note:
 *  For convenience, you can just give up the current time slice.
 */
/*** exercise 4.6 ***/
void sys_yield(void)
{
	bcopy((void *)KERNEL_SP - sizeof(struct Trapframe),
	      (void *)TIMESTACK - sizeof(struct Trapframe),
	      sizeof(struct Trapframe));
	//printf("yield!\n");
	sched_yield();
}

/* Overview:
 * 	This function is used to destroy the current environment.
 *
 * Pre-Condition:
 * 	The parameter `envid` must be the environment id of a
 * process, which is either a child of the caller of this function
 * or the caller itself.
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 when error occurs.
 */
int sys_env_destroy(int sysno, u_int envid)
{
	/*
	   printf("[%08x] exiting gracefully\n", curenv->env_id);
	   env_destroy(curenv);
	 */
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0) {
		return r;
	}

	printf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

int sys_thread_destroy(int sysno, u_int threadid)
{
	int r;
	struct Thread *t;
	if ((r = threadid2thread(threadid, &t)) < 0) {
		return r;
	}
	if (t->thread_status == ENV_FREE) {
		return -E_INVAL;
	}
	//t->thread_exit_value = 0;
	struct Thread *tmp;
	int empty = LIST_EMPTY(&t->thread_joined_list);
	while (!LIST_EMPTY(&t->thread_joined_list)) {
		tmp = LIST_FIRST(&t->thread_joined_list);
		LIST_REMOVE(tmp, thread_joined_link);
		//*(tmp->thread_join_value_ptr) = t->thread_exit_ptr;
		printf("notified threadid is %d\n", THREADX(tmp->thread_id));
		sys_set_thread_status(0, tmp->thread_id, ENV_RUNNABLE);
	}
	printf("[%08x] destroying thread %08x\n", curenv->env_id, t->thread_id);
	thread_destroy(t);
	return 0;
}

/* Overview:
 * 	Set envid's pagefault handler entry point and exception stack.
 *
 * Pre-Condition:
 * 	xstacktop points to one byte past exception stack.
 *
 * Post-Condition:
 * 	The envid's pagefault handler will be set to `func` and its
 * 	exception stack will be set to `xstacktop`.
 * 	Returns 0 on success, < 0 on error.
 */
/*** exercise 4.12 ***/
int sys_set_pgfault_handler(int sysno, u_int envid, u_int func, u_int xstacktop)
{
	// Your code here.
	struct Env *env;
	int ret;

	if ((ret = envid2env(envid, &env, 0)) < 0) {
		return ret;
	}

	env->env_pgfault_handler = func;
	env->env_xstacktop = xstacktop;

	return 0;
	//      panic("sys_set_pgfault_handler not implemented");
}

/* Overview:
 * 	Allocate a page of memory and map it at 'va' with permission
 * 'perm' in the address space of 'envid'.
 *
 * 	If a page is already mapped at 'va', that page is unmapped as a
 * side-effect.
 *
 * Pre-Condition:
 * perm -- PTE_V is required,
 *         PTE_COW is not allowed(return -E_INVAL),
 *         other bits are optional.
 *
 * Post-Condition:
 * Return 0 on success, < 0 on error
 *	- va must be < UTOP
 *	- env may modify its own address space or the address space of its children
 */
/*** exercise 4.3 ***/
int sys_mem_alloc(int sysno, u_int envid, u_int va, u_int perm)
{
	// Your code here.
	struct Env *env;
	struct Page *ppage;
	int ret;
	ret = 0;

	if (!(perm & PTE_V) || (perm & PTE_COW)) {
		return -E_INVAL;
	}
	if (va >= UTOP) {
		return -E_INVAL;
	}

	ret = envid2env(envid, &env, 1);
	if (ret < 0) {
		return ret;
	}

	ret = page_alloc(&ppage);
	if (ret < 0) {
		return ret;
	}

	ret = page_insert(env->env_pgdir, ppage, va, perm);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

/* Overview:
 * 	Map the page of memory at 'srcva' in srcid's address space
 * at 'dstva' in dstid's address space with permission 'perm'.
 * Perm must have PTE_V to be valid.
 * (Probably we should add a restriction that you can't go from
 * non-writable to writable?)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Note:
 * 	Cannot access pages above UTOP.
 */
/*** exercise 4.4 ***/
int sys_mem_map(int sysno, u_int srcid, u_int srcva, u_int dstid, u_int dstva,
		u_int perm)
{
	int ret;
	u_int round_srcva, round_dstva;
	struct Env *srcenv;
	struct Env *dstenv;
	struct Page *ppage;
	Pte *ppte;

	ppage = NULL;
	ret = 0;
	round_srcva = ROUNDDOWN(srcva, BY2PG);
	round_dstva = ROUNDDOWN(dstva, BY2PG);

	//your code here
	if (srcva >= UTOP || dstva >= UTOP) {
		return -E_INVAL;
	}

	if (!(perm & PTE_V)) {
		return -E_INVAL;
	}

	if ((ret = envid2env(srcid, &srcenv, 0)) < 0) {
		return ret;
	}

	if ((ret = envid2env(dstid, &dstenv, 0)) < 0) {
		return ret;
	}

	ppage = page_lookup(srcenv->env_pgdir, round_srcva, &ppte);
	if (ppage == NULL) {
		return -E_INVAL;
	}

	if (!(*ppte & PTE_R) && (perm & PTE_R)) {
		return -E_INVAL;
	}

	if ((ret =
	     page_insert(dstenv->env_pgdir, ppage, round_dstva, perm)) < 0) {
		return ret;
	}
	return 0;
}

/* Overview:
 * 	Unmap the page of memory at 'va' in the address space of 'envid'
 * (if no page is mapped, the function silently succeeds)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Cannot unmap pages above UTOP.
 */
/*** exercise 4.5 ***/
int sys_mem_unmap(int sysno, u_int envid, u_int va)
{
	// Your code here.
	int ret;
	struct Env *env;

	if (va >= UTOP) {
		return -E_INVAL;
	}

	if ((ret = envid2env(envid, &env, 1)) < 0) {
		return ret;
	}

	page_remove(env->env_pgdir, va);

	return 0;
	//      panic("sys_mem_unmap not implemented");
}

/* Overview:
 * 	Allocate a new environment.
 *
 * Pre-Condition:
 * The new child is left as env_alloc created it, except that
 * status is set to ENV_NOT_RUNNABLE and the register set is copied
 * from the current environment.
 *
 * Post-Condition:
 * 	In the child, the register set is tweaked so sys_env_alloc returns 0.
 * 	Returns envid of new environment, or < 0 on error.
 */
/*** exercise 4.8 ***/
int sys_env_alloc(void)
{
	// Your code here.
	int r;
	struct Env *e;
	if ((r = env_alloc(&e, curenv->env_id)) < 0) {
		return r;
	}
	bcopy((void *)KERNEL_SP - sizeof(struct Trapframe),
	      (void *)&(e->env_threads[0].thread_tf), sizeof(struct Trapframe));
	//e->env_tf.pc = e->env_tf.cp0_epc;
	e->env_threads[0].thread_tf.pc = e->env_threads[0].thread_tf.cp0_epc;
	//e->env_status = ENV_NOT_RUNNABLE;
	e->env_threads[0].thread_status = ENV_NOT_RUNNABLE;
	e->env_pri = curenv->env_pri;
	e->env_threads[0].thread_pri = curenv->env_pri;
	e->env_threads[0].thread_tf.regs[2] = 0;

	return e->env_id;
	//      panic("sys_env_alloc not implemented");
}

int sys_thread_alloc(void)
{
	int r;
	struct Thread *t;

	if ((r = thread_alloc(curenv, &t)) < 0) {
		return r;
	}
	t->thread_pri = curenv->env_pri;
	t->thread_status = ENV_NOT_RUNNABLE;
	t->thread_tf.regs[2] = 0;
	return THREADX(t->thread_id);
}

/* Overview:
 * 	Set envid's env_status to status.
 *
 * Pre-Condition:
 * 	status should be one of `ENV_RUNNABLE`, `ENV_NOT_RUNNABLE` and
 * `ENV_FREE`. Otherwise return -E_INVAL.
 *
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if status is not a valid status for an environment.
 * 	The status of environment will be set to `status` on success.
 */
/*** exercise 4.14 ***/
int sys_set_env_status(int sysno, u_int envid, u_int status)
{
	// Your code here.
	struct Env *env;
	struct Thread *thread;
	int ret;

	if (status != ENV_RUNNABLE && status != ENV_NOT_RUNNABLE
	    && status != ENV_FREE) {
		return -E_INVAL;
	}

	if ((ret = envid2env(envid, &env, 0)) < 0) {
		return ret;
	}

	thread = &env->env_threads[0];
	if (status == ENV_RUNNABLE && thread->thread_status != ENV_RUNNABLE) {
		//printf("insert %x\n", thread->thread_id);
		LIST_INSERT_HEAD(&thread_sched_list[0], thread,
				 thread_sched_link);
	} else if (status != ENV_RUNNABLE
		   && thread->thread_status == ENV_RUNNABLE) {
		//printf("remove %x\n", thread->thread_id);
		LIST_REMOVE(thread, thread_sched_link);
	}
	thread->thread_status = status;
	return 0;
	//      panic("sys_env_set_status not implemented");
}

int sys_set_thread_status(int sysno, u_int threadid, u_int status)
{
	struct Thread *t;
	int r;
	if ((status != ENV_RUNNABLE) && (status != ENV_NOT_RUNNABLE)
	    && (status != ENV_FREE)) {
		return -E_INVAL;
	};
	if ((r = threadid2thread(threadid, &t)) < 0) {
		return r;
	}
	if ((status == ENV_RUNNABLE) && (t->thread_status != ENV_RUNNABLE)) {
		//printf("insert %x\n", t->thread_id);
		LIST_INSERT_HEAD(thread_sched_list, t, thread_sched_link);
	} else if ((status != ENV_RUNNABLE)
		   && (t->thread_status == ENV_RUNNABLE)) {
		//printf("remove %x\n", t->thread_id);
		LIST_REMOVE(t, thread_sched_link);
	}
	t->thread_status = status;
	return 0;

}

/* Overview:
 * 	Set envid's trap frame to tf.
 *
 * Pre-Condition:
 * 	`tf` should be valid.
 *
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if the environment cannot be manipulated.
 *
 * Note: This hasn't be used now?
 */
int sys_set_trapframe(int sysno, u_int envid, struct Trapframe *tf)
{

	return 0;
}

/* Overview:
 * 	Kernel panic with message `msg`.
 *
 * Pre-Condition:
 * 	msg can't be NULL
 *
 * Post-Condition:
 * 	This function will make the whole system stop.
 */
void sys_panic(int sysno, char *msg)
{
	// no page_fault_mode -- we are trying to panic!
	panic("%s", TRUP(msg));
}

/* Overview:
 * 	This function enables caller to receive message from
 * other process. To be more specific, it will flag
 * the current process so that other process could send
 * message to it.
 *
 * Pre-Condition:
 * 	`dstva` is valid (Note: 0 is also a valid value for `dstva`).
 *
 * Post-Condition:
 * 	This syscall will set the current process's status to
 * ENV_NOT_RUNNABLE, giving up cpu.
 */
/*** exercise 4.7 ***/
void sys_ipc_recv(int sysno, u_int dstva)
{
	if (dstva >= UTOP) {
		return;
	}
	if (curenv->env_ipc_recving == 1) {
		sys_yield();
	}
	curenv->env_ipc_recving = 1;
	curenv->env_ipc_dstva = dstva;
	curenv->env_ipc_waiting_thread_no = THREADX(curthread->thread_id);
	curthread->thread_status = ENV_NOT_RUNNABLE;

	sys_yield();
}

/* Overview:
 * 	Try to send 'value' to the target env 'envid'.
 *
 * 	The send fails with a return value of -E_IPC_NOT_RECV if the
 * target has not requested IPC with sys_ipc_recv.
 * 	Otherwise, the send succeeds, and the target's ipc fields are
 * updated as follows:
 *    env_ipc_recving is set to 0 to block future sends
 *    env_ipc_from is set to the sending envid
 *    env_ipc_value is set to the 'value' parameter
 * 	The target environment is marked runnable again.
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Hint: You need to call `envid2env`.
 */
/*** exercise 4.7 ***/
int sys_ipc_can_send(int sysno, u_int envid, u_int value, u_int srcva,
		     u_int perm)
{

	int r;
	struct Env *e;
	struct Thread *t;
	struct Page *p;
	if (srcva >= UTOP) {
		return -E_INVAL;
	}

	if ((r = envid2env(envid, &e, 0)) < 0) {
		return r;
	}

	if (e->env_ipc_recving == 0) {
		return -E_IPC_NOT_RECV;
	}

	e->env_ipc_value = value;
	e->env_ipc_from = curenv->env_id;
	e->env_ipc_perm = perm;
	e->env_ipc_recving = 0;
	t = &e->env_threads[e->env_ipc_waiting_thread_no];
	t->thread_status = ENV_RUNNABLE;

	if (srcva != 0) {
		Pte *pte;
		p = page_lookup(curenv->env_pgdir, srcva, &pte);
		if (p == NULL) {
			return -E_INVAL;
		}
		page_insert(e->env_pgdir, p, e->env_ipc_dstva, perm);
	}

	return 0;
}

int sys_thread_join(int sysno, u_int threadid, void **retval)
{
	struct Thread *t;
	int r;
	//printf("here id is 0x%x\n",threadid);
	//printf("find id is 0x%x\n",t->thread_id);
	if ((r = threadid2thread(threadid, &t)) < 0) {
		return r;
	}
	//printf("r = %d\n", r);
	if (t->thread_detach) {
		return -E_THREAD_JOIN_FAIL;
	}
	if (t->thread_status == ENV_FREE) {
		if (retval != 0) {
			*retval = t->thread_exit_ptr;
		}
		return 0;
	}

	sys_set_thread_status(0, curthread->thread_id, ENV_NOT_RUNNABLE);
	LIST_INSERT_TAIL(&t->thread_joined_list, curthread, thread_joined_link);
	curthread->thread_join_value_ptr = retval;
	int empty = LIST_EMPTY(&t->thread_joined_list);
	//printf("no.%d'sempty = %d\n", THREADX(t->thread_id), empty);
	struct Trapframe *trap =
	    (struct Trapframe *)(KERNEL_SP - sizeof(struct Trapframe));
	trap->regs[2] = 0;
	trap->pc = trap->cp0_epc;
	//bcopy((struct Trapframe *)(KERNEL_SP - sizeof(struct Trapframe)),
	//        (void*) &(curthread->thread_tf), sizeof(struct Trapframe));
	//printf("joined!\n");
	sys_yield();
	return 0;
}

int sys_sem_destroy(int sysno, sem_t * sem)
{
	if ((sem->sem_envid != curenv->env_id) && (sem->sem_shared == 0)) {
		return -E_SEM_NOT_FOUND;
	}
	if (sem->sem_status == SEM_FREE) {
		return 0;
	}
	sem->sem_status = SEM_FREE;
	return 0;
}

int sys_sem_wait(int sysno, sem_t * sem)
{
	if (sem->sem_status == SEM_FREE) {
		//printf("it's done\n");
		return -E_SEM_ERROR;
	}
	int i;
	if (sem->sem_value > 0) {
		--sem->sem_value;
		return 0;
	}
	if (sem->sem_wait_num >= 10) {
		printf("toop many\n");
		return -E_SEM_ERROR;
	}
	LIST_INSERT_TAIL(&sem->sem_wait_list, curthread, thread_wait_link);
	++sem->sem_wait_num;
	sys_set_thread_status(0, 0, ENV_NOT_RUNNABLE);
	struct Trapframe *trap =
	    (struct Trapframe *)(KERNEL_SP - sizeof(struct Trapframe));
	trap->regs[2] = 0;
	trap->pc = trap->cp0_epc;
	sys_yield();
	return 0;
}

int sys_sem_trywait(int sysno, sem_t * sem)
{
	if (sem->sem_status == SEM_FREE) {
		return -E_SEM_ERROR;
	}
	if (sem->sem_value > 0) {
		--sem->sem_value;
		return 0;
	}
	return -E_SEM_FAILED_TO_WAIT;
}

int sys_sem_post(int sysno, sem_t * sem)
{
	struct Thread *t;
	if (sem->sem_status == SEM_FREE) {
		return -E_SEM_ERROR;
	}
	if (sem->sem_value > 0) {
		++sem->sem_value;
	} else {
		if (sem->sem_wait_num == 0) {
			++sem->sem_value;
		} else {
			--sem->sem_wait_num;
			t = LIST_FIRST(&sem->sem_wait_list);
			LIST_REMOVE(t, thread_wait_link);
			sys_set_thread_status(0, t->thread_id, ENV_RUNNABLE);
		}
	}
	return 0;
}

int sys_sem_getvalue(int sysno, sem_t * sem, int *retval)
{
	if (sem->sem_status == SEM_FREE) {
		//printf("it's done2\n");
		return -E_SEM_ERROR;
	}
	if (retval != 0) {
		*retval = sem->sem_value;
	}
	return 0;
}
