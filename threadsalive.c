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
ucontext_t mainctx; //global variable to keep track of ctx for main and a linked list of ctx's
struct node* list;


void ta_libinit(void) {
	list=NULL;  //have the list be null to start
    return;
}

void ta_create(void (*func)(void *), void *arg) {
	unsigned char *stack1 = (unsigned char*) malloc(128000); // a stack for the ucontext_t

	ucontext_t ctx1;  // initializing and creating our ctx for our func
	getcontext(&ctx1);
	ctx1.uc_stack.ss_sp = stack1;
	ctx1.uc_stack.ss_size = 128000;
	ctx1.uc_link = &mainctx;
	makecontext(&ctx1, (void (*) (void)) func, 1, arg);  // creates the thread and puts it on the list
	
	
	list_append(&ctx1, &list);

	
    return;
}

void ta_yield(void) {
	//if list is empty then just return
	if (list== NULL) {
		printf("list is null\n");
		return;
		}
	//make new nodes have one be the front of the list and one be the back
	struct node *front = list;
	struct node *back = list;
	
	while (back->next!=NULL) {
		back=back->next;
		}
	back->next=front; //move the list so the front is the back and the second is the first now
	list=list->next;
	
	front->next= NULL;
	
		swapcontext(&front->ctx, &list->ctx);  //change control to the next thread
		

}

int ta_waitall(void) {
	if (list== NULL) {
		return 0;  //if there is nothing on the list then just return
		}
	struct node *current_thread= list; // make a new node to track the list
	while (1) {	
		swapcontext(&mainctx, &(current_thread->ctx)); // swap from the main to the different threads
		if (current_thread->next ==NULL) { // if there is nothing left in the queue then just return
			return 0;
			}
		current_thread=current_thread->next; // move the thread to be the next one
		
		
		}
	printf("List is Null\n");

	
    return -1;
}


/* ***************************** 
     stage 2 library functions
   ***************************** */

void ta_sem_init(tasem_t *sem, int value) { // need to set semaphore value to value and allocate for the list of sems
	sem->value=value;
	sem->sems= malloc(sizeof (struct node));

}

void ta_sem_destroy(tasem_t *sem) {// free the sems on the stack
	
	free(sem->sems);
}

void ta_sem_post(tasem_t *sem) { // add one to the sem value. 
	sem->value +=1;
	if (sem->sems != NULL) { // if there are other things waiting and it is over zero then just continue
		if (sem->value >=0) {
			
			return;
			}
		ta_yield();
		return;
		}
	ta_yield();
	return;
			

}

void ta_sem_wait(tasem_t *sem) {
	while (sem->value<0) {//yield until the value is greater than 1 then decrement and continue
		ta_yield();
		}
	sem->value-=1;
		
}

void ta_lock_init(talock_t *mutex) { //set the semaphore to 1 (unlocked) and malloc for the sems 
	mutex->lock.value=1;
	mutex->lock.sems=malloc(sizeof (struct node));
}
void ta_lock_destroy(talock_t *mutex) {// free the sems
	free(mutex->lock.sems);
}

void ta_lock(talock_t *mutex) { // while the lock is 0(locked) yield the processor
	while (mutex->lock.value==0) {
		ta_yield();
		}
	mutex->lock.value=0; // when it is 1 (unlocked) then change it to zero and keep going
	return;
	
	
}

void ta_unlock(talock_t *mutex) { //when finished your critical section sets to 1 (unlocked)
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

