#include "lib.h"
#include <error.h>

int sem_init(sem_t * sem, int shared, unsigned int value)
{
	if (sem == 0) {
		return -E_SEM_ERROR;
	}
	LIST_INIT(&sem->sem_wait_list);
	sem->sem_envid = env->env_id;
	sem->sem_value = value;
	sem->sem_shared = shared;
	sem->sem_status = SEM_VALID;
	sem->sem_wait_num = 0;
	return 0;
}

int sem_destroy(sem_t * sem)
{
	return syscall_sem_destroy(sem);
}

int sem_wait(sem_t * sem)
{
	return syscall_sem_wait(sem);
}

int sem_trywait(sem_t * sem)
{
	return syscall_sem_trywait(sem);
}

int sem_post(sem_t * sem)
{
	return syscall_sem_post(sem);
}

int sem_getvalue(sem_t * sem, int *retval)
{
	return syscall_sem_getvalue(sem, retval);
}
