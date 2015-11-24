#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "bookorders.h"
#include "order.h"
#include "queue.h"

/* my user database */

Database db;

/* my category list */

char **categories;

/* number of categories in list, helps free up the space and create total amount of threads */

int count;

/* my shared buffer queue */

queue *orders;

/* boolean flag to tell consumer that the producer is done */

int complete=0;

/* build up the categories array based on the categories.txt file */

int buildCategories(char *filename)
{
	FILE *catfile= fopen(filename, "r");
	count =0;
	
	if(catfile==NULL)
	{
		return -1;
	}
	
	char cat_line[1024];
	
	while(fgets(cat_line, 1024, catfile)!=NULL)
	{
		count++;
	}
	
	rewind(catfile);
	
	categories = malloc(sizeof(char*)*count+1);
	
	int index=0;
	
	while(fgets(cat_line, 1024, catfile)!=NULL)
	{
		categories[index] = malloc(sizeof(char)*strlen(cat_line)+1);
		strcpy(categories[index] , cat_line);
		index++;
	}
	
	fclose(catfile);
	
	return 0;
}

void* producer(void *arg)
{
	FILE *orderss = fopen((char*) arg, "r");
	
	char *title;
	char *category;
	
	if(orderss==NULL)
	{
		printf("Unable to open the orders file\n");
		exit(EXIT_FAILURE);
	}
	
	/* need to tokenize each line of the order and send it to the queue for the consumer to eventually pick up */
	
	char order_line[1024];
	
	while(fgets(order_line,1024, orderss)!=NULL)
	{
		struct Order *request = malloc(sizeof(struct Order));
		
		/* Book Title */
		
		title= strtok(order_line,"|\"");
		
		request->title= malloc(sizeof(char)*strlen(title)+1);
		strcpy(request->title, title); 
		
		/* Price */
		
		request->price = atof(strtok(NULL,"|\""));
		
		/* Customer Id */
		
		request->id= atoi(strtok(NULL,"|\""));
		
		/* Category */
		
		category = strtok(NULL, "|\"");
		request->category = malloc(sizeof(char)*strlen(category)+1);
		strcpy(request->category, category);
		
		/* Enqueue the order into the shared buffer */
		
		/* consumer will also enqueue successful/failed orders so I need to mutex lock */
		
		pthread_mutex_lock(&orders->mutex);
		
		if(checkBufferSize(orders)>=10)
		{
			printf("Producer waits because queue is full.\n\n");
			pthread_cond_wait(&orders->check_full, &orders->mutex);
		}
		
		int orig_size =checkBufferSize(orders);
		
		enqueue(orders, request);
		pthread_mutex_unlock(&orders->mutex);
		
		/* Conditional signal to blocked consumer that queue isn't empty anymore */
		
		if(orig_size==0)
		{
			printf("Consumer resumes when queue has orders ready for processing\n\n");
		}
		
		pthread_cond_signal(&orders->check_empty);
	}
	
	/* Broadcast now to remaining blocked threads to finish off the orders */
	
	complete=1;
	
	pthread_cond_broadcast(&orders->check_empty);
	
	fclose(orderss); 
	
	return 0;
}

void* consumer(void *arg)
{
	/* identify category of the thread */
	
	char *interpret = (char*)arg;
	char *category = malloc(sizeof(char)*strlen(interpret)+1);
	strcpy(category, interpret);
	
	/* I need to keep in mind the relay between producer and consumer. Let's keep looping while the producer isn't done */
	
	while(complete==0 || amIEmpty(orders)==0)
	{
		/* lock until this point so there isn't a clash among the threads */
		
		pthread_mutex_lock(&orders->mutex);
		
		/* queue is empty and the producer isn't finished. I need to wait for more input. Cond wait automatically unlocks */
		
		if(complete==0 && amIEmpty(orders)==1)
		{
			printf("Consumer waits because queue is empty.\n\n");
			pthread_cond_wait(&orders->check_empty, &orders->mutex);
		}
		
		/* what if producer is done and the queue is empty? Let's exit */
		
		if(complete==1 && amIEmpty(orders)==1)
		{
			pthread_mutex_unlock(&orders->mutex);
			free(category);
			return 0;
		}
		
		/* what happens if a simulaneous thread makes the queue empty? Yield until producer enqueue stuff */
		
		else if(amIEmpty(orders)==1) 
		{
			pthread_mutex_unlock(&orders->mutex);
			sched_yield();
			continue;
		}
		
		/* try to process the order now by making sure the order to dequeue is the same category */
		
		struct Order *top_cat = seeFront(orders);
		
		/* not the same category, yield to another thread */
		
		if(strcmp(top_cat->category, category)!=0)
		{
			pthread_mutex_unlock(&orders->mutex);
			sched_yield();
		}
		
		else if(strcmp(top_cat->category, category)==0)
		{
			/* dequeue the order */
			
			struct Order *new_order = dequeue(orders);
			
			if(checkBufferSize(orders)==9)
			{
				printf("Producer resumes when queue space is available\n\n");
				pthread_cond_signal(&orders->check_full);
			}
			
			/* get the customer from the database */
			
			Customer *identified = findperson(new_order->id, db);
			
			if(identified==NULL)
			{
				printf("Couldn't find customer with that id number.\n");
			}
			else
			{
				/* Create a receipt and figure out which situation it belongs to */
				
				double difference = identified->credit_limit - new_order->price;
				
				Receipt *stub = createStub(new_order->title, new_order->price, difference);
				
				/* If remaining credit of user is not sufficient */
				
				if(identified->credit_limit<new_order->price)
				{
					printf("%s doesn't have sufficient funds to purchase: \n Book Name: %s \n Book Price: %g \n Remaining credit limit of: %g\n\n", identified->full_name, new_order->title, new_order->price, identified->credit_limit);
					insertReceipt(identified->bad, stub);
				}
				
				/* What if he has sufficient funds? */
				
				else if(identified->credit_limit>=new_order->price)
				{
					identified->credit_limit = identified->credit_limit - new_order->price;
					
					printf("Customer %s has sufficient funds, purchase information is below:\n", identified->full_name);
					printf("Book Name: %s\n", new_order->title);
					printf("Book Price: %g\n", new_order->price);
					printf("Shipping information: \n Name: %s \n Address: %s \n State: %s \n Zip Code: %s \n\n", identified->full_name, identified->street_address, identified->state, identified->zip_code);
					insertReceipt(identified->good, stub);
				}
			}
			
			free(new_order->title);
			free(new_order->category);
			free(new_order);
			pthread_mutex_unlock(&orders->mutex);
		}
	}
	
	free(category);
	return 0;
}

int generateDB(char *filename)
{
	/* open a database.txt file, and read each line by line, store the info in each Customer struct */
	
	FILE *customers = fopen(filename, "r");
	
	if(customers==NULL)
	{
		return -1;
	}
	
	char customer_line[1024];
	
	db=malloc(sizeof(struct CustomerList));
	db->head= NULL;
	
	char *holder;
	
	int length;
	
	while(fgets(customer_line, 1024, customers)!=NULL)
	{
		/* 1. Allocate memory for the customer object 
		 * 2. Use String tokenizer to obtain the info for each part. */
		 
		Customer *person = malloc(sizeof(Customer));
		person->next= NULL;
		person->good=malloc(sizeof(Receipts));
		person->good->head=NULL;
		person->bad= malloc(sizeof(Receipts));
		person->bad->head=NULL; 
		
		/* Full Name */
		
		holder =strtok(customer_line,"|\"");
		
		length = strlen(holder);
		
		person->full_name= malloc(sizeof(char)*length+1);
		
		strcpy(person->full_name, holder);
		
		/* Customer Id */
		
		holder =strtok(NULL,"|\"");
		
		person->id_number = atoi(holder);
		
		/* Credit Limit */
		
		holder = strtok(NULL, "|\"");
		person->credit_limit = atof(holder);
		
		/* Street Address */
		
		holder = strtok(NULL, "|\"");
		length= strlen(holder);
		person->street_address = malloc(sizeof(char)*length+1);
		strcpy(person->street_address, holder);
		
		/* State */
		
		holder = strtok(NULL, "|\"");
		length= strlen(holder);
		person->state = malloc(sizeof(char)*length+1);
		strcpy(person->state, holder);
		
		/* Zip Code */
		
		holder = strtok(NULL, "|\"");
		length= strlen(holder);
		person->zip_code= malloc(sizeof(char)*length+1);
		strcpy(person->zip_code, holder);
		
		/* Add this person to the database linked list */
		
		if(db->head==NULL)
		{
			db->head = person;
		}
		else if(db->head!=NULL)
		{
			Customer *temp = db->head;
			Customer *prev = db->head;
			
			while(temp!=NULL)
			{
				prev=temp;
				temp=temp->next;
			}
			
			prev->next= person;			
		}
	}
	
	fclose(customers);
	
	return 0;
}

void DestroyDatabase(Database list)
{
	if(list==NULL)
	{
		return;
	}
	
	if(list->head==NULL)
	{
		free(list);
		return;
	}
	
	Customer *temp = list->head;
	
	while(temp!=NULL)
	{
		Customer *hold=temp;
		temp=temp->next;
		free(hold->full_name);
		free(hold->street_address);
		free(hold->state);
		free(hold->zip_code);
		DestroyReceipts(hold->good);
		DestroyReceipts(hold->bad);
		free(hold);
	}
	
	free(list);
} 

void writeReport(Database users)
{
	if(users==NULL || users->head==NULL)
	{
		printf("No customers to report.\n");
		return;
	}
	
	char *filename = "finalreport.txt";
	
	FILE *openfile = fopen(filename, "w");
	
	Customer *temp = users->head;
	double revenue=0.0;
	
	while(temp!=NULL)
	{
		fprintf(openfile, "=== BEGIN CUSTOMER INFO ===\n");
		fprintf(openfile, "### BALANCE ###\n");
		fprintf(openfile, "Customer name: %s\n", temp->full_name);
		fprintf(openfile, "Customer ID number: %d\n", temp->id_number);
		fprintf(openfile, "Remaining credit balance after all purchases (a dollar amount): %g\n", temp->credit_limit);
		fprintf(openfile, "### SUCCESSFUL ORDERS ###\n");
		
		Receipt *tempo = temp->good->head;
		
		/* SUCCESSFUL ORDERS */
		
		while(tempo!=NULL)
		{
			fprintf(openfile, "\"%s\"|", tempo->book_title);
			fprintf(openfile, "%g|", tempo->book_price);
			fprintf(openfile, "%g\n", tempo->remain_credit);
			
			revenue=revenue+tempo->book_price;

			tempo= tempo->next;
		}
		
		fprintf(openfile, "### REJECTED ORDERS ###\n");
		
		Receipt *tempon = temp->bad->head;
		
		/* UNSUCCESSFUL ORDERS */
		
		while(tempon!=NULL)
		{
			fprintf(openfile, "\"%s\"|", tempon->book_title);
			fprintf(openfile, "%g|\n", tempon->book_price);
			
			tempon= tempon->next;
		}
		
		fprintf(openfile, "=== END CUSTOMER INFO ===\n\n");
		temp=temp->next;
	}
	
	/* REVENUE */
	
	fprintf(openfile, "Total Revenue: %g\n", revenue);
	
	fclose(openfile);
}
int main (int argc, char **argv)
{
	/* input is as follows:
	 * 
	 * ./bookorders database.txt orders.txt categories.txt */
	 
	if(argc!=4)
	{
		printf("Error: Invalid amount of inputs.\n\n");
		printf("Proper input is as follows: \n\n");
		printf("./bookorders database.txt orders.txt categories.txt \n\n");
		return 0;
	}
	
	/* set up the categories from the file in an array */
	
	int catresult =buildCategories(argv[3]);
	
	if(catresult==-1)
	{
		printf("Error: Category file wasn't read properly\n");
		return 0;
	}
	
	/* Need to build a database, make it a linked list of users */
	
	char *user_file_name= argv[1];
	
	int dbresult =generateDB(user_file_name);
	
	if(dbresult==-1)
	{
		printf("Failed to open the database file.\n");
		return 0;
	}
	
	/* set up the shared buffer queue */
	
	orders= createQ();
	
	/* set up the total amount of pthreads:
	 * 1 for producer
	 * 1 consumer thread per category */
	 
	 int total= count+1;
	 
	 pthread_t threads[total];
	 
	 /* set up producer thread */
	 
	 pthread_create(&threads[0], NULL, producer, argv[2]);
	 
	 /* set up consumer threads for each category */
	
	int i;
	int j=0;
	void *filler;
	
	for(i=1; i<total;i++)
	{
		pthread_create(&threads[i], NULL, consumer, categories[j]);
		j++;
	}
	
	/* Before printing out the final report, let's join the threads to make sure they are done */
	
	for(i=0; i<total; i++)
	{
		pthread_join(threads[i], &filler);
	}
	 /* print and write the final report */
	 
	 writeReport(db);
	 
	/* free up used space */
	
	DestroyDatabase(db);
	destroyCategories(categories);
	destroyQ(orders);
	
	printf("Please refer to finalreport.txt for the final report of this program run.\n");
	
	return 0;
}

/* testing purposes */

void printDataStructure(Customer *hold)
{
	printf("Name: %s\n", hold->full_name);
	printf("Price: %d\n", hold->id_number);
	printf("ID: %f\n", hold->credit_limit);
	printf("category: %s\n" ,hold->street_address);	
}
void destroyCategories(char **list)
{
	int i;
	
	for(i=0; i<count; i++)
	{
		free(list[i]);
	}
	
	free(list);
}

void DestroyReceipts(Receipts list)
{
	if(list==NULL)
	{
		return;
	}
	
	if(list->head==NULL)
	{
		free(list);
		return;
	}
	
	Receipt *temp=list->head;
	
	while(temp!=NULL)
	{
		Receipt *hold=temp;
		temp=temp->next;
		free(hold->book_title);
		free(hold);
	}
	
	free(list);
}
