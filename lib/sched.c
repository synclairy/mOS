#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'thread_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.15 ***/
extern struct Thread_list thread_sched_list[];
extern struct Thread *curthread;

void sched_yield(void)
{
	static int count = 1;	// remaining time slices of current env
	static int point = 0;	// current thread_sched_list index
	/*  hint:
	 *  1. if (count==0), insert `e` into `thread_sched_list[1-point]`
	 *     using LIST_REMOVE and LIST_INSERT_TAIL.
	 *  2. if (thread_sched_list[point] is empty), point = 1 - point;
	 *     then search through `thread_sched_list[point]` for a runnable env `e`, 
	 *     and set count = e->env_pri
	 *  3. count--
	 *  4. env_run()
	 *
	 *  functions or macros below may be used (not all):
	 *  LIST_INSERT_TAIL, LIST_REMOVE, LIST_FIRST, LIST_EMPTY
	 */
	struct Thread *next_thread;
	count--;
	int found;
	if (count <= 0 || curthread == NULL
	    || curthread->thread_status != ENV_RUNNABLE) {
		found = 0;
		if (curthread != NULL
		    && curthread->thread_status == ENV_RUNNABLE) {
			LIST_REMOVE(curthread, thread_sched_link);
			LIST_INSERT_TAIL(&thread_sched_list[!point], curthread,
					 thread_sched_link);
		}
		while (1) {
			if (LIST_EMPTY(&thread_sched_list[point])) {
				point = !point;
				break;
			}
			next_thread = LIST_FIRST(&thread_sched_list[point]);
			//          printf("1:point = %d, threadid = %x,satus = %d\n", point, next_thread->thread_id, next_thread->thread_status);
			if (next_thread->thread_status == ENV_RUNNABLE) {
				found = 1;
				break;
			}
			LIST_REMOVE(next_thread, thread_sched_link);
			LIST_INSERT_TAIL(&thread_sched_list[!point],
					 next_thread, thread_sched_link);
		}
		if (!found) {
			while (1) {
				if (LIST_EMPTY(&thread_sched_list[point])) {
					panic
					    ("^^^^^^No env is RUNNABLE!^^^^^^");
				}
				next_thread =
				    LIST_FIRST(&thread_sched_list[point]);
//                printf("2:point = %d, threadid = %x,satus = %d\n", point, next_thread->thread_id, next_thread->thread_status);
				if (next_thread->thread_status == ENV_RUNNABLE) {
					found = 1;
					break;
				}
				LIST_REMOVE(next_thread, thread_sched_link);
				LIST_INSERT_TAIL(&thread_sched_list[!point],
						 next_thread,
						 thread_sched_link);
			}
		}
		count = next_thread->thread_pri;
		env_run(next_thread);
	}
	env_run(curthread);
	panic("^^^^^^sched yield reached end^^^^^^");
}
