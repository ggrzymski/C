#include <stdlib.h>
#include <stdio.h>
#include "sorted-list.h"
#include <string.h>

/* Big O: Run-time is O(1) because I am directly creating a structure without iteration */

SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df)
{
	if(cf==NULL)
	{
		return NULL;
	}
	
	/* Since SortedListPtr is typedefined as a pointer, the malloced space needs to be called
	 * on the size of the actual struct, which is struct SortedList. My CompareFunct function
	 * point will point to the caller's function, cf. I also need to initialize the list to
	 * NULL to avoid seg faults. */
	 
	SortedListPtr sorted_list= malloc(sizeof(struct SortedList));
	
	sorted_list->head=NULL;
	sorted_list->CompareFuncT=cf;
	sorted_list->DestructFuncT=df;
	
	return sorted_list;
}

/* Big O: Run-time is O(n) because every node in the list needs to be freed */

void SLDestroy(SortedListPtr list)
{
	/* My sorted list has a linked list of struct Nodes that need to be freed
	 * individually. Create a temp node to hold the current Node, traverse to the next node,
	 * and then call free on another Node that holds temp. List must also be freed. */
	 
	 if(list==NULL)
	 {
		 return;
	 }
	 
	 if(list->head==NULL)
	 {
		 free(list);
		 return;
	 }
	 
	 Node *temp= list->head;
	 
	 while(temp!=NULL)
	 {
		 Node *hold=temp;
		 temp=temp->next;
		 list->DestructFuncT(hold->data); /* DestFunct call instead of free from now on */
		 free(hold);
	 }
	 
	 free(list);
}

/* Big O: Worst Case Run-time is O(n), when you have to check each item in the list and then add new item at the end. */

int SLInsert(SortedListPtr list, void *newObj)
{
	/* make sure inputs are valid */
	
	if(list==NULL || newObj ==NULL)
	{
		return 0;
	}
	
	/*get a count of how many elements are in the list already */
	
	Node *temp= list->head;
	int obj_count=0;
	
	while(temp!=NULL)
	{
		obj_count++;
		temp=temp->next;
	}
	
	/* check for duplicates */
	
	temp= list->head;
	int dup_count=0;
	int cmp_result=0;
	
	while(temp!=NULL)
	{
		cmp_result= list->CompareFuncT(temp->data,newObj);
		
		if(cmp_result==0)
		{
			dup_count=1;
		}
		
		temp=temp->next;
	}
	
	if (dup_count==1) /* Found a duplicate */
	{
		list->DestructFuncT(newObj);
		return 0;
	}
	
	Node *new_object= (Node*)malloc(sizeof(Node));
	new_object->data=newObj;
	new_object->ref_count=1;
	new_object->flag=0;
	
	/* If the SortedList head is non-existent, create a new head */
	
	if(list->head==NULL)
	{
		list->head=new_object;
		list->head->ref_count=1;
		list->head->next=NULL;
		return 1;
	}
	
	/* reset temp and check for cases of greater than, less than.
	 * Remember to maintain descending order. 5->4->3->2->1 */
	 
	 temp= list->head;
	 Node *prev=list->head;
	 cmp_result=0;
	 
	 /* Check for the test case where there is one element */
	 
	 if(obj_count==1)
	 {
		 cmp_result=list->CompareFuncT(newObj,list->head->data);
		 
		 if (cmp_result<0)  /* new object is smaller, ref count already 1 */
		 {
			 list->head->next=new_object;
			 new_object->next=NULL;
			 return 1;
		 }
		 
		 if(cmp_result>0) 
		 {
			/* new object is bigger. Store the header information in a new Node,
			 * have new object point to the new new Node, free old header node,
			 * and have the non-malloced header point to the new object */
			
			Node *temp_hold=list->head;
			new_object->next=temp_hold;
			list->head=new_object;
			 
			return 1;  
		 }
	 }
	 
	  /* Traverse the list until you encounter newObj being greater than something */
	  
	  while(temp!=NULL)
	 {
		 cmp_result=list->CompareFuncT(newObj,temp->data);
		 
		 if(cmp_result<0)
		 {
	 		 prev=temp;
			 temp=temp->next;
		 }
		 else if(cmp_result>0)
		 {
			 break;
	     }	 
	 }
	 
	 /* Case 1: newObj is less than everything on the list. */
	 
	 if(temp==NULL)
	 {
		 prev->next=new_object;
		 new_object->next=NULL;
		 return 1;
	 }
	 
	 /* Case 2: newObj is greater than everything in the list */
	 
	 if (temp==list->head)
	 {
			Node *temp_hold=list->head;
			new_object->next=temp_hold;
			list->head=new_object;
			return 1;  
	 }
	 
	 /* Case 3: newObj is less than some but greater than something */
	 
	 new_object->next=prev->next;
	 prev->next=new_object;
	 
	 return 1;
}

/* Big O Runtime: This function may have O(n) where the last item on the list needs to be deleted. */

int SLRemove(SortedListPtr list, void *newObj)
{
	if(list==NULL || newObj==NULL)
	{
		return 0;
	}
	
	/* Case 1: Empty Linked List */
	
	if(list->head==NULL)
	{
		return 0;
	}
	
	/* Get Amount of elements in the list */
	
	Node *traverse=list->head;
	int total_count=0;
	
	while(traverse!=NULL)
	{
		total_count++;
		traverse=traverse->next;
	}
	
	/* Case 2: newObj doesn't exist in the list */
	
	Node *temp= list->head;
	int confirm=0;
	int cmp_result=0;
	
	while(temp!=NULL)
	{
		cmp_result= list->CompareFuncT(newObj,temp->data);
		
		if(cmp_result==0) /*Found its existence in the list */
		{
			confirm=1;
		}
		
		temp=temp->next;
	}
	
	if(confirm==0) /* get rid of the malloced item */
	{
		list->DestructFuncT(newObj);
		return 0;
	}
	
	/* Case 3: What if Head is the node to be Removed? Need to make the 
	 * next pointer be the new Head */
	 
	 cmp_result=list->CompareFuncT(newObj,list->head->data);
	 
	 if(cmp_result==0 && total_count==1) /* Only Node on the list */
	 {
		 list->head->ref_count--; /* if equal to 1, then iterator still pointing to it, can't free. */
		 list->head->flag=1;
		 
		 if(list->head->ref_count==0)
		 {
			list->DestructFuncT(list->head->data);
			free(list->head);
			list->head=NULL;
		 }
		 
		 return 1;
	 }
	 
	 if(cmp_result==0 && total_count>1)
	 {
		 list->head->ref_count--;
		 list->head->flag=1;
		 int checker=list->head->ref_count;
		 Node *tempo=list->head;
		 list->head=list->head->next;
			
		 if(checker==0)
		 {
			list->DestructFuncT(tempo->data);
			free(tempo);	
		 }
		 
		 /* Else, head is pointed by iterator and needs to be removed in SLNextItem */
		 
		 return 1;
	 }
	 
	 /* Case 4: Tail and Middle Elements need to be Removed. */
	 
	 temp=list->head;
	 Node *prev=list->head;
	 cmp_result=0;
	 
	 while(temp!=NULL)
	 {
		 cmp_result= list->CompareFuncT(newObj, temp->data);
		 
		 if(cmp_result==0)
		 {
			 break;
		 }
		 prev=temp;
		 temp=temp->next;
	 }
	 
	 temp->ref_count--;
	 temp->flag=1;
	 prev->next=temp->next;
	 
	 if(temp->ref_count==0)
	 {
		list->DestructFuncT(temp->data);
		free(temp);
	}
	 
	return 1;
}

/* Big O Runtime: O(1) because it is a direct creation of a struct */

SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
	if(list==NULL || list->head==NULL)
	{
		return NULL; /* there is nothing to iterate through */
	}
	
	SortedListIteratorPtr iterate= malloc(sizeof(struct SortedListIterator));
	
	iterate->track=list->head;
	iterate->DestructFuncT=list->DestructFuncT;
	
	if(iterate->track!=NULL)
	{
		iterate->track->ref_count++; /* Make it equal to higher value */
	}
	
	return iterate;
}

/* Big O Runtime: This is O(1) because it is a direct deletion of the iterator */

void SLDestroyIterator(SortedListIteratorPtr iter)
{
	/* I need to make sure the node pointed by the iterator has its reference count
	 * decremented by 1. It should only reach 1 at the minimum. */
	
	if(iter->track!=NULL)
	{
		iter->track->ref_count--;
	}
	
	 free(iter);
	 
}

/* Big O Runtime: This is O(1) because it directly returns the item that was pointed by the iterator */

void *SLNextItem(SortedListIteratorPtr iter)
{
	/* I want to return the current object pointed by Iterator. I also want to
	 * move the iterator to the next node.
	 * 
	 * I need to make sure that I free Nodes with a ref count of 0 and pointed to 
	 * by iterator since a SLRemove was called on it beforehand. */
	 
	if(iter==NULL || iter->track==NULL)
	{
		return NULL; /* invalid input or when reaching end of line */
	}
	
	iter->track->ref_count--;
	
	if(iter->track->ref_count!=0 && iter->track->flag==1) /* more than one iterator on a removed node */
	{
		iter->track=iter->track->next;
		
		if(iter->track==NULL)
		{
			return NULL;
		}
		
		void *data_object=iter->track->data;
		
		iter->track=iter->track->next;
		
		if(iter->track!=NULL)
		{
			iter->track->ref_count++;
		}
		
		return data_object;
	}
	
	if(iter->track->ref_count==0) /* needs to be removed and have the iterator moved to the next item */ 
	{
		Node *temp=iter->track;
		Node *next_node=iter->track->next;
		
		iter->DestructFuncT(temp->data);
		free(temp);
		temp=NULL;
		
		iter->track=next_node;
		
		if(iter->track==NULL)
		{
			return NULL; /* meaning we reached the end of the sorted list */
		}
	}
	
	void *data_object= iter->track->data;
	
	iter->track=iter->track->next; /* move to the next element */
	
	if(iter->track!=NULL)
	{
		iter->track->ref_count++; /* Bring the new node up to another reference */
	}
	
	return data_object;	
}
