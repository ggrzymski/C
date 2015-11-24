#ifndef SORTED_LIST_H
#define SORTED_LIST_H
/*
 * sorted-list.h
 */

#include <stdlib.h>
#include <stdio.h>

/* Represents my File Record Nodes */

struct FileNode
{
	char *record; /* file name */
	int frequency; /* how many times does the name appear in file */
	struct FileNode *next;
};

/* Represents my file name sorted list */

struct FileSortedList
{
	struct FileNode *head;
};

typedef struct FileSortedList* FilePtr;

typedef struct FileNode Record;

/* Represents my Node for a token */

struct WordNode
{
	char *word; /* token */
	struct WordNode *next; 
	FilePtr records; /* my list of file name records */
};

typedef struct WordNode Word;

/* Represents my tracker of the WordNode List */

struct SortedList
{
	struct WordNode *head;
	int (*CompareFuncT)(void *, void *); /* compareWords */
};

typedef struct SortedList* WordPtr;

typedef int (*CompareFuncT)(void *, void *);

WordPtr SLCreate(CompareFuncT cf); /* create my token linked list */
FilePtr FileCreate(); /* create my file name linked list for every new token */

void SLDestroy(WordPtr list); /* free malloced memory*/
void FileDestroy(FilePtr list);

int SLInsert(WordPtr list, char *token, char *file_name); /* attempt to insert a new token */
int FileInsert(FilePtr list, char *file_name); /* attempt to insert a new file record */

#endif
