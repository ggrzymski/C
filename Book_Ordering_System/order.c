#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "order.h"
#include "queue.h"

/* find the person that is associated with the input id */

Customer *findperson(int id, Database db)
{
	Customer *temp = db->head;
	
	while(temp!=NULL)
	{
		if(id==temp->id_number)
		{
			return temp;
		}
		
		temp=temp->next;
	}
	return NULL;
}

/* create a Receipt */

Receipt *createStub(char *title, double price, double credit)
{
	if(title==NULL)
	{
		return NULL;
	}
	
	Receipt *stub = malloc(sizeof(struct receipt));
	
	stub->book_title = malloc(sizeof(char)*strlen(title)+1);
	strcpy(stub->book_title, title);
	stub->book_price = price;
	stub->remain_credit = credit;
	
	return stub;
}

/*insert a receipt either into successful or unsuccessful linked list */

int insertReceipt(Receipts list, Receipt *item)
{
	if(list==NULL)
	{
		return -1;
	}
	
	if(list->head==NULL)
	{
		list->head = item;
		list->head->next = NULL;
	}
	else
	{
		Receipt *prev= list->head;
		Receipt *temp= list->head;
		
		while(temp!=NULL)
		{
			prev=temp;
			temp=temp->next;
		}
		
		prev->next = item;
		item->next= NULL;
	}
	
	return 0;
}
