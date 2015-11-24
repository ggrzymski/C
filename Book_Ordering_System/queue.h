#ifndef QUEUE_H
#define QUEUE_H

/*
 * queue.h
 */
 
typedef struct Queue
{
	struct Order *tail; /* fifo behavior */
	pthread_mutex_t mutex; /* regulate consumer/producer behavior */
	pthread_cond_t check_empty; /* checks if anything is in queue and signals producer/consumer */
	pthread_cond_t check_full; /* checks if the buffer size of 10 has its limit respected */
	
}queue;

queue *createQ();
void destroyQ(queue *deleteMe);
int enqueue(queue *list, struct Order *item);
struct Order* dequeue(queue *list);
int amIEmpty(queue *list);
struct Order* seeFront(queue *list);
int checkBufferSize(queue *list);

#endif
