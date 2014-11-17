#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>

/* your list function definitions */





void list_append(ucontext_t *ctx1, struct node**head){
	struct node *new = malloc(sizeof(struct node));
	new->ctx = *ctx1;
	new->next= NULL; 

	//if head is null, make the node the head
	if(*head == NULL) {
		
		*head = new;
		return;
	}
	struct node *iterator = *head;
	
	
	while(iterator->next != NULL) {
		iterator = iterator->next;
	}


	iterator->next=new;
	return;
}

void list_print(struct node *list) {
	int count=0;
	//cycle through each element and print it out...
    	while (list != NULL) {
        	printf("%d\n", count);
        	list = list->next;
		count++;
    	}
}

void list_delete(struct node *list) {
	//cycle through each element and delete it.
    	while (list != NULL) {
        	struct node *tmp = list;
        	list = list->next;
        	free(tmp);
   	}
}


