#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sorted-list.h"
#include "tokenizer.h"
#include "index.h"

WordPtr SLCreate(CompareFuncT cf)
{
	if(cf==NULL)
	{
		return NULL;
	}
	
	/* Since WordPtr is typedefined as a pointer, the malloced space needs to be called
	 * on the size of the actual struct, which is struct SortedList. My CompareFunct function
	 * point will point to the caller's function, cf. I also need to initialize the list to
	 * NULL to avoid seg faults. */
	 
	WordPtr sorted_list= malloc(sizeof(struct SortedList));
	
	sorted_list->head=NULL;
	sorted_list->CompareFuncT=cf;
	
	return sorted_list;
}

/* Creating my file record list */

FilePtr FileCreate()
{
	FilePtr sorted_list= malloc(sizeof(struct FileSortedList));
	
	sorted_list->head=NULL;
	
	return sorted_list;
}

/* freeing all the token nodes in my linked list */

void SLDestroy(WordPtr list)
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
	 
	 Word *temp= list->head;
	 
	 while(temp!=NULL)
	 {
		 Word *hold=temp;
		 temp=temp->next;
		 free(hold->word);
		 FileDestroy(hold->records); /* need to free the records before freeing the node */
		 free(hold);
	 }
	 
	 free(list);
}

/* freeing all the file name records */

void FileDestroy(FilePtr list)
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
	 
	 Record *temp= list->head;
	 
	 while(temp!=NULL)
	 {
		 Record *hold=temp;
		 temp=temp->next;
		 free(hold->record);
		 free(hold);
	 }
	 
	 free(list);
}
int SLInsert(WordPtr list, char *token, char *file_name)
{
	/* I am inserting a token node into the linked list. If the node already exists,
	 * then I make a call to FileInsert to increase the frequency count of the file. Otherwise,
	 * the node gets created with also its file list created. */
	 
	 /* token= name of word.
	  * file_name= name of file currently being processed. */
	  
	 if(list==NULL || token==NULL || file_name==NULL)
	 {
		 return 0;
	 }
	 
	/*get a count of how many elements are in the list already */
	
	Word *temp= list->head;
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
		cmp_result= list->CompareFuncT(temp->word,token);
		
		if(cmp_result==0)
		{
			dup_count=1;
			break;
		}
		
		temp=temp->next;
	}
	
	/* Case 1: Word already exists on the list */
	
	if (dup_count==1) /* Found a duplicate */
	{
		/* The token already exists on the list. Call on the FileInsert method
		 * to increase the count of the File record with the file name. */
		 
		 free(token);
		 FileInsert(temp->records, file_name);
		 return 1;
	}
	
	/* Case 2: Word doesn't exist yet on the list. */
	
	Word *new_object= (Word*)malloc(sizeof(Word));
	new_object->word=token;
	new_object->records=FileCreate();
	new_object->next=NULL;
	
	/* If the WordPtr head is non-existent, create a new head */
	
	if(list->head==NULL)
	{
		list->head=new_object;
		FileInsert(list->head->records, file_name);
		return 1;
	}
	
	/* reset temp and check for cases of greater than, less than.
	 * Remember to maintain ascending order. 1->2->3->4->5 */
	 
	 temp= list->head;
	 Word *prev=list->head;
	 cmp_result=0;
	 
	 /* Check for the test case where there is one element */
	 
	 if(obj_count==1)
	 {
		 cmp_result=list->CompareFuncT(token,list->head->word);
		 
		 if (cmp_result<0)  /* new object is smaller */
		 {
			 /* Store the header information in a new Word,
			 * have new object point to the new new Word, free old header node,
			 * and have the non-malloced header point to the new object */
			 
			 Word *temp_hold=list->head;
			 new_object->next=temp_hold;
			 FileInsert(new_object->records, file_name);
			 list->head=new_object;
			 
			 return 1; 
		 }
		 
		 if(cmp_result>0) 
		 {
			 list->head->next=new_object;
			 FileInsert(new_object->records, file_name);
			 new_object->next=NULL;
			 return 1;
		 }
	 }
	 /* Traverse the list until you encounter newObj being less than something */
	  
	  while(temp!=NULL)
	 {
		 cmp_result=list->CompareFuncT(token,temp->word);
		 
		 if(cmp_result<0)
		 {
	 		 break;
		 }
		 else if(cmp_result>0)
		 {
			prev=temp;
			temp=temp->next;
	     }	 
	 }
	 
	 /* Case 1: word is greater than everything on the list. */
	 
	 if(temp==NULL)
	 {
		 prev->next=new_object;
		 FileInsert(new_object->records, file_name);
		 new_object->next=NULL;
		 return 1;
	 }
	 
	 /* Case 2: word is less than everything in the list */
	 
	 if (temp==list->head)
	 {
			Word *temp_hold=list->head;
			new_object->next=temp_hold;
			FileInsert(new_object->records, file_name);
			list->head=new_object;
			return 1;  
	 }
	 
	/* Case 3: word is greater than some but less than something */
	 
	 new_object->next=prev->next;
	 FileInsert(new_object->records, file_name);
	 prev->next=new_object;
	 
	 return 1;
}

int FileInsert(FilePtr list, char *file_name)
{
	/* File Name could either exist already(increment frequency count)
	 * or need to be inserted into the list. The list follows a descending order
	 * pattern based on the frequency numer */
	 
	if(list==NULL || file_name ==NULL)
	{
		return 0;
	}
	
	/*get a count of how many elements are in the list already */
	
	Record *temp= list->head;
	int obj_count=0;
	
	while(temp!=NULL)
	{
		obj_count++;
		temp=temp->next;
	}
	
	/* Case 1: File already exists */
	
	temp= list->head;
	Record *prev=list->head;
	int dup_count=0;
	int cmp_result=0;
	
	while(temp!=NULL)
	{
		cmp_result= compareWords(temp->record,file_name);
		
		if(cmp_result==0)
		{
			dup_count=1;
			break;
		}
		
		prev=temp;
		temp=temp->next;
	}
	
	if (dup_count==1) /* Found the record */
	{
		/* Head is the one whose frequency is increased */
		
		if(temp==list->head)
		{
			free(file_name);
			list->head->frequency++;
			return 1;
		}
		
		/* File is either a Middle or Tail Node */
		
		temp->frequency++;
		
		Record *holder=(Record*)malloc(sizeof(Record));
		
		char *testing=malloc(sizeof(char)*strlen(temp->record)+1);
		strcpy(testing,temp->record);
		holder->record= testing;
		holder->frequency=temp->frequency;
		holder->next=NULL;
		
		/* I need to get it out of the list first */
		
		prev->next=temp->next;
		free(temp->record);
		free(temp);
		
		Record *iterate= list->head;
		Record *previous= list->head;
		
		/* Traverse the list until you encounter holder's frequency being greater than something */
		
		 while(iterate!=NULL)
		 {
			cmp_result=compareInts(holder->frequency,iterate->frequency);
		 
			if(cmp_result<0)
			{
				previous=iterate;
				iterate=iterate->next;
			}
			
			else if(cmp_result>0 || cmp_result==0)
			{
				break;
			}	 
		 }
		 
		 /* Case 1: holder is greater than everything in the list or equal to the head's frequency. */
		 
		 if (iterate==list->head)
		 {
				Record *temp_hold=list->head;
				holder->next=temp_hold;
				list->head=holder;
				free(file_name);
				return 1;  
		 }
		 
		 /* Case 2: newObj is less than some but greater than something */
		 
		 holder->next=previous->next;
		 previous->next=holder;
		 free(file_name);
		 
		 return 1;
	}
	
	/* If the record wasn't found, then you can create a new one with a frequency of 1
	 * and add it to the end of the list. */
	 
	 Record *new_object= (Record*)malloc(sizeof(Record));
	 new_object->record=file_name;
	 new_object->frequency=1;
	 new_object->next=NULL;
	 
	 /* If the FilePtr head is non-existent, create a new head */
	
	if(list->head==NULL)
	{
		list->head=new_object;
		return 1;
	}
	
	temp=list->head;
	prev=list->head;
	
	while(temp!=NULL)
	{
		prev=temp;
		temp=temp->next;
	}
	
	prev->next=new_object;
	new_object->next=NULL;
	
	return 1;
}
	
