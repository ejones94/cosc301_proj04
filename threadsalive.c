/*
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h>
#include <string.h>

#include "threadsalive.h"
#include <ucontext.h>
#include "list.c"
#include "list.h"



/* ***************************** 
     stage 1 library functions
   ***************************** */
ucontext_t mainctx;
struct node* list;


void ta_libinit(void) {
	list=NULL;
	getcontext(&mainctx);
    return;
}

void ta_create(void (*func)(void *), void *arg) {
	unsigned char *stack1 = (unsigned char*) malloc(128000);

	ucontext_t ctx1;
	getcontext(&ctx1);
	ctx1.uc_stack.ss_sp = stack1;
	ctx1.uc_stack.ss_size = 128000;
	ctx1.uc_link = &mainctx;
	makecontext(&ctx1, (void (*) (void)) func, 1, arg);
	
	
	list_append(&ctx1, &list);

	free(stack1);
    return;
}

void ta_yield(void) {

	if (list== NULL) {
		return;
		}
	
	struct node *current = list;
	struct node *back = list;
	while (back->next != NULL) {
		back= back->next;
		}
	back->next=current;
	list= current->next;
	current->next = NULL; 
	swapcontext(&current->ctx, &list ->ctx);
	
    return;
}

int ta_waitall(void) {
	if (list== NULL) {
		return 0;
		}
	struct node *current_thread= list;
	while (1) {
		swapcontext(&mainctx, &current_thread->ctx);
		if (current_thread->next ==NULL) {
			return 0;
			}
		current_thread= current_thread->next;
		
		
		}
	printf("List is Null\n");

	
    return -1;
}


/* ***************************** 
     stage 2 library functions
   ***************************** */

void ta_sem_init(tasem_t *sem, int value) {
	sem->value=value;
	sem->sems= malloc(sizeof (struct node));

}

void ta_sem_destroy(tasem_t *sem) {
	
	free(sem->sems);
}

void ta_sem_post(tasem_t *sem) {
	sem->value +=1;
	if (sem->sems != NULL) {
		if (sem->value >=0) {
			return;
			}
		
		return;
		}
	return;
			

}

void ta_sem_wait(tasem_t *sem) {
	while (sem->value<0) {
		ta_yield();
		}
	sem->value-=1;
		
}

void ta_lock_init(talock_t *mutex) {
	mutex->lock.value=1;
	mutex->lock.sems=malloc(sizeof (struct node));
}
void ta_lock_destroy(talock_t *mutex) {
	free(mutex->lock.sems);
}

void ta_lock(talock_t *mutex) {
	while (mutex->lock.value==0) {
		ta_yield();
		}
	mutex->lock.value=0;
	return;
	
	
}

void ta_unlock(talock_t *mutex) {
	mutex->lock.value=1;
	return;
	
}


/* ***************************** 
     stage 3 library functions
   ***************************** */

void ta_cond_init(tacond_t *cond) {
}

void ta_cond_destroy(tacond_t *cond) {
}

void ta_wait(talock_t *mutex, tacond_t *cond) {
}

void ta_signal(tacond_t *cond) {
}

