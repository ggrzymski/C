#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>
#include <pthread.h>
#include "order.h"

/* creating a circular linked list that mimics a queue. I want to make it one shared
buffer between the producer and the consumer, and I must remember to keep it to a limit of 10
orders in the linked list */

queue *createQ()
{
	queue *orders = malloc(sizeof(struct Queue));
	
	orders->tail=NULL;
	
	/* make sure mutex initializes properly */
	
	if(pthread_mutex_init(&orders->mutex, NULL)!=0)
	{
		free(orders);
		return NULL;
	}
	
	/* same goes for the condition variables */
	
	if(pthread_cond_init(&orders->check_empty, NULL)!=0)
	{
		free(orders);
		return NULL;
	}
	
	if(pthread_cond_init(&orders->check_full, NULL)!=0)
	{
		free(orders);
		return NULL;
	}
	
	return orders;
}

void destroyQ(queue *deleteMe)
{
	if(deleteMe==NULL)
	{
		return;
	}
	
	/* I need to also worry about the mutex and condition variables */
	
	pthread_mutex_destroy(&deleteMe->mutex);
	pthread_cond_destroy(&deleteMe->check_empty);
	pthread_cond_destroy(&deleteMe->check_full);
	
	if(deleteMe->tail==NULL)
	{
		free(deleteMe);
		return;
	}
	
	/* One node in the list */
	
	if(deleteMe->tail == deleteMe->tail->next)
	{
		free(deleteMe->tail->title);
		free(deleteMe->tail->category);
		free(deleteMe->tail);
		free(deleteMe);
		return;
	}
	
	/* Multiple node deletions */
	
	struct Order *temp= deleteMe->tail->next;
	
	while(temp!=deleteMe->tail)
	{
		struct Order *hold = temp;
		temp=temp->next;
		
		free(hold->title); /* name of book */
		free(hold->category); /* category it belongs to */
		free(hold);
	}
	
	free(deleteMe->tail->title); 
	free(deleteMe->tail->category);
	free(deleteMe->tail);
	free(deleteMe);	
}

/* insert an order into the queue as the new tail, bump the old tail to the left. */

int enqueue(queue *list, struct Order *item)
{
	if(list==NULL)
	{
		return -1;
	}
	
	if(list->tail==NULL)
	{
		list->tail= item;
		list->tail->next = list->tail;
	}
	else
	{
		/* make it the new tail of the list */
		
		item->next= list->tail->next;
		list->tail->next = item;
		list->tail= item;
	}
	
	return 0;
}

/* take out the order from the front of the queue because that was the first one that came in */

struct Order* dequeue(queue *list)
{
	/* make sure it isn't empty */
	
	if(list==NULL || list->tail==NULL)
	{
		return NULL;
	}
	
	struct Order *one_order = malloc(sizeof(struct Order));
	
	/* What if there's only one item in the list? */
	
	if(list->tail->next==NULL || list->tail == list->tail->next)
	{
		one_order->title= malloc(sizeof(char)*strlen(list->tail->title)+1);
		strcpy(one_order->title,list->tail->title);
		one_order->price= list->tail->price;
		one_order->id= list->tail->id;
		one_order->category= malloc(sizeof(char)*strlen(list->tail->category)+1);
		strcpy(one_order->category,list->tail->category);
		one_order->next= list->tail->next;
		
		free(list->tail->title);
		free(list->tail->category);
		free(list->tail);
		list->tail=NULL;
		
		return one_order;
	}
	
	/* take it from the front of the list */
	
	struct Order *hold = list->tail->next;
	
	one_order->title= malloc(sizeof(char)*strlen(list->tail->next->title)+1);
	strcpy(one_order->title,list->tail->next->title);
	one_order->price= list->tail->next->price;
	one_order->id= list->tail->next->id;
	one_order->category= malloc(sizeof(char)*strlen(list->tail->next->category)+1);
	strcpy(one_order->category,list->tail->next->category);
	one_order->next= list->tail->next->next;
	
	
	list->tail->next=list->tail->next->next;
	
	free(hold->title);
	free(hold->category);
	free(hold);
	
	return one_order;
}

/* checks the occupancy of the queue, helps with consumer/producer communication
 * 1 means it is empty, 0 means it has something. */

int amIEmpty(queue *list)
{
	if(list->tail == NULL || list==NULL)
	{
		return 1;
	}
	
	return 0;
}

/* check the front of the list to see which category it belongs to */

struct Order* seeFront(queue *list)
{
	/* one node in the list */
	
	if(list==NULL)
	{
		return NULL;
	}
	
	if(list->tail!=NULL && list->tail->next!=NULL)
	{
		return list->tail->next;
	}
	
	return NULL;
}

/* reports the amount of current items in the buffer */

int checkBufferSize(queue *list)
{
	if(list==NULL || list->tail==NULL)
	{
		return 0;
	}
	
	if(list->tail == list->tail->next || list->tail->next==NULL)
	{
		return 1;
	}
	
	struct Order *temp = list->tail->next;
	int count=0;
	
	while(temp!=list->tail)
	{
		count++;
		temp=temp->next;
	}
	
	count=count+1; /* I need to include the tail as part of the count */
	
	return count;
}

