/*
 * sorted-list.c
 */

#include <string.h>
#include "sorted-list.h"
#include <stdlib.h>
#include <stdio.h>

int compareInts(void *p1, void *p2)
{
	int i1 = *(int*)p1;
	int i2 = *(int*)p2;

	return i1 - i2;
}

int compareDoubles(void *p1, void *p2)
{
	double d1 = *(double*)p1;
	double d2 = *(double*)p2;

	return (d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0);
}

int compareStrings(void *p1, void *p2)
{
	char *s1 = p1;
	char *s2 = p2;

	return strcmp(s1, s2);
}

//Destructor functions
void destroyBasicTypeAlloc(void *p)
{
	//For pointers to basic data types (int*,char*,double*,...)
	//Use for allocated memory (malloc,calloc,etc.)
	free(p);
}

void destroyBasicTypeNoAlloc(void *p) 
{
	//For pointers to basic data types (int*,char*,double*,...)
	//Use for memory that has not been allocated (e.g., "int x = 5;SLInsert(mylist,&x);SLRemove(mylist,&x);")
	return;
}

int main()
{
	int option=0;
	
	while(option!=15)
	{
		printf("There are 14 Test Cases\n");
		printf("Enter Test Case Numer to Print Out:\n");
		printf("Press 15 if you want to quit\n");
		
		scanf("%d",&option);
		
		if(option==1)
		{
			printf("CompareInts, tests for duplicates and Descended Order\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			int *num_6=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			*num_6=6;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			SLInsert(sl,(void*)num_6);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			while(iterate->track!=NULL)
			{	
				print_out=SLNextItem(iterate);
		
				printf("%d\n", *(int*)print_out);
			}
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);		
		}
		if(option==2)
		{
			printf("compareInts, checks for proper deletion of middle nodes\n");
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			int *num_6=malloc(sizeof(int));
			int *num_7=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			*num_6=6;
			*num_7=7;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			SLInsert(sl,(void*)num_6);
			SLInsert(sl,(void*)num_7);
			
			SLRemove(sl,(void*)num_4);
			SLRemove(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			while(iterate->track!=NULL)
			{	
				print_out=SLNextItem(iterate);
		
				printf("%d\n", *(int*)print_out);
			}
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);		
		}
		if(option==3)
		{
			printf("compareInts, checks for proper removal of head node\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLRemove(sl,(void*)num_2);
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			while(iterate->track!=NULL)
			{	
				print_out=SLNextItem(iterate);
		
				printf("%d\n", *(int*)print_out);
			}
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);	
		}
		if(option==4)
		{
			printf("compareInts, checks for proper removal of tail node\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLRemove(sl,(void*)num_1);
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			while(iterate->track!=NULL)
			{	
				print_out=SLNextItem(iterate);
		
				printf("%d\n", *(int*)print_out);
			}
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);	
		}
		if(option==5)
		{
			printf("compareInts, checks for proper deletion and re-insertion\n");
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SLRemove(sl,(void*)num_3);
			SLRemove(sl,(void*)num_4);
			SLRemove(sl,(void*)num_5);
			
			num_3=malloc(sizeof(int));
			num_4=malloc(sizeof(int));
			num_5=malloc(sizeof(int));
			
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			while(iterate->track!=NULL)
			{	
				print_out=SLNextItem(iterate);
		
				printf("%d\n", *(int*)print_out);
			}
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		if(option==6)
		{
			printf("CompareInts, SLNextItem x 2, SLRemove, SLNextItem x 3\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			print_out=SLNextItem(iterate);
			print_out=SLNextItem(iterate);
			
			SLRemove(sl,(void*)num_4);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);		
		}
		if(option==7)
		{
			printf("CompareInts, 1 x SLRemove, 3 x SLNextItem \n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			print_out=SLNextItem(iterate);
			print_out=SLNextItem(iterate);
			
			SLRemove(sl,(void*)num_2);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			if(print_out==NULL)
			{
				printf("NULL\n");
			}
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		if(option==8)
		{
			printf("CompareInts, 1 x SLInsert, 4 x SLNextItem\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_0=malloc(sizeof(int));
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			
			*num_0=0;
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			print_out=SLNextItem(iterate);
			print_out=SLNextItem(iterate);
			
			SLInsert(sl,num_0);
			print_out=SLNextItem(iterate);
			printf("%d\n",*(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		if(option==9)
		{
			printf("CompareInts, SLRemoving the Iterator Pointer\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			print_out=SLNextItem(iterate);
			print_out=SLNextItem(iterate);
			
			SLRemove(sl,num_3);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		if(option==10)
		{
			printf("CompareInts, SLInserting in front of iterator\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			int *num_6=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			*num_6=6;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			print_out=SLNextItem(iterate);
			printf("%d\n",*(int*)print_out);
			SLInsert(sl,num_6);
			print_out=SLNextItem(iterate);
			printf("%d\n",*(int*)print_out);
			
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		if(option==11)
		{
			printf("CompareInts, SLRemove during iteration\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			print_out=SLNextItem(iterate);
			print_out=SLNextItem(iterate);
			
			SLRemove(sl,num_2);
			
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		if(option==12)
		{
			printf("CompareInts, Destroying an Iterator, and Creating a New One on the list with a new element\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			int *num_6=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			*num_6=6;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			print_out=SLNextItem(iterate);
			print_out=SLNextItem(iterate);
			
			SLDestroyIterator(iterate);
			SLInsert(sl,num_6);
			iterate=SLCreateIterator(sl);
			
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		if(option==13)
		{
			printf("CompareInts, SLRemoving the Head Node pointed by the Iterator\n");
			
			SortedListPtr sl=SLCreate(compareInts,destroyBasicTypeAlloc);
			
			int *num_1=malloc(sizeof(int));
			int *num_2=malloc(sizeof(int));
			int *num_3=malloc(sizeof(int));
			int *num_4=malloc(sizeof(int));
			int *num_5=malloc(sizeof(int));
			
			*num_1=1;
			*num_2=2;
			*num_3=3;
			*num_4=4;
			*num_5=5;
			
			SLInsert(sl,(void*)num_1);
			SLInsert(sl,(void*)num_2);
			SLInsert(sl,(void*)num_3);
			SLInsert(sl,(void*)num_4);
			SLInsert(sl,(void*)num_5);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			void *print_out;
			
			SLRemove(sl,num_5);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			print_out=SLNextItem(iterate);
			printf("%d\n", *(int*)print_out);
			SLDestroyIterator(iterate);
			SLDestroy(sl);
		}
		
		if(option==14)
		{
			printf("CompareStrings, tests for two iterators one one list.\n");

			SortedListPtr sl=SLCreate(compareStrings,destroyBasicTypeNoAlloc);
			
			char a_orig[2]="a";
			char b_orig[2]="b";
			char c_orig[2]="c";
			char d_orig[2]="d";
			
			SLInsert(sl,(void*)a_orig);
			SLInsert(sl,(void*)b_orig);
			SLInsert(sl,(void*)c_orig);
			SLInsert(sl,(void*)d_orig);
			
			SortedListIteratorPtr iterate=SLCreateIterator(sl);
			SortedListIteratorPtr iterate2= SLCreateIterator(sl);
			
			void *print_out;

			print_out=SLNextItem(iterate); /* point to c */ 
			print_out=SLNextItem(iterate); /* point to b */
			print_out=SLNextItem(iterate2); /* point to c */
			
			SLRemove(sl,b_orig);
			SLRemove(sl,c_orig);
			
			print_out= SLNextItem(iterate);
			printf("%s\n",(char*)print_out);
			
			print_out=SLNextItem(iterate2);
			printf("%s\n",(char*)print_out);

			
			SLDestroyIterator(iterate);
			SLDestroyIterator(iterate2);
			
			SLDestroy(sl);
		}
	}
	
	return 0;
}
