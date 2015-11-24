#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "tokenizer.h"
#include "sorted-list.h"
#include "index.h"

/* This represents my linked list of mapped tokens, with every token containing a linked list of records. */

WordPtr inverted_list;

/* Compare name tokens */

int compareWords(void *p1, void *p2)
{
	char *w1= p1;
	char *w2= p2;
	
	return strcmp(w1, w2);
}

/* Compare frequency count of files */

int compareInts(int p1, int p2)
{
	int i1= p1;
	int i2= p2;
	
	return (i1 < i2) ? -1 : ((i1 > i2) ? 1 : 0);
}

/* The processing of argv[2], which could either contain a file or directory. The directory is recursively searched through by utilizing
 * struct dirent tools. A tokenizer is created if the file is a text document, and processed through the linked list. */
 
int process(char * file)
{
	struct stat file_check;
	DIR *directory;
	
	/* check status of file */
	
	if(stat(file, &file_check)==-1)
	{
		fprintf(stderr, "Error: File/Folder doesn't exist\n");
		return 0;
	}
	
	/* if file or folder is legit, need to check which one is it with stat mode.
	 * If it is a directory, I need to recurse it. */
	 
	 /* A directory */
	 
	if(S_ISDIR(file_check.st_mode))
	{
		/* Need to keep going down until I reach files. Use a directory stream */
		
		directory=opendir(file);
		
		if(directory==NULL)
		{
			printf("Error, couldn't open directory.\n");
			return 0;
		}
		else
		{
			processdir(file, directory);
			closedir(directory);
		}
	}
	
	/* A file */
	
	if(S_ISREG(file_check.st_mode))
	{
		/* Proces the file by utilizing tokenizer and linked list. */
		
		TokenizerT *tokened= TKCreate(file);
		
		if(tokened==NULL)
		{
			printf("Error: couldn't tokenize file properly.\n");
			return 0;
		}
		
		char *token;
		char *word;
		char *file_name;
		
		while((token=TKGetNextToken(tokened))!=NULL)
		{
			word=(char*)malloc(sizeof(char)*strlen(token)+1);
			strcpy(word,token);
			file_name=malloc(sizeof(char*)*strlen(file)+1);
			strcpy(file_name,file);
			
			SLInsert(inverted_list, word, file_name);
			
			free(token);	
		}
		
		TKDestroy(tokened);
	}
	 
	return 0;
}

void processdir(char *main, DIR *directory)
{
	struct dirent *file;
	char *pathname;
	int slash=1;
	
	int length= strlen(main);
	
	if(main[length-1]!='/') /* I need to manually add a slash to my file name */
	{
		slash=2;
	}
	
	while((file=readdir(directory))!=NULL)
	{
		if(strcmp(file->d_name,".")!=0 && strcmp(file->d_name,"..")!=0) /* avoid the parent */
		{
			pathname=(char*)malloc(sizeof(char)*(strlen(main)+strlen(file->d_name)+slash));
			
			strcpy(pathname,main);
			
			/* need to add slash to end */
			
			if(slash==2)
			{
				strcat(pathname,"/");
			}
			
			strcat(pathname,file->d_name);
			strcat(pathname,"\0"); /* remember, it's a string */
			
			process(pathname);
			free(pathname);
		}
	}
}

int writefile(WordPtr list,char *output)
{
	Word *name_temp= list->head;
	Record *file_temp;
	
	int count=0; /* keep track of printable records */
	
	FILE *openfile= fopen(output,"w");
	
	while(name_temp!=NULL)
	{
		fprintf(openfile, "<list> %s\n",(char*)name_temp->word);
		
		file_temp=name_temp->records->head;
		count=0;
		
		while(file_temp!=NULL)
		{
			if(count<5)
			{
				fprintf(openfile,"%s %d ", (char*)file_temp->record,file_temp->frequency);
				count++;
			}
			if(count==5)
			{
				if(file_temp->next!=NULL) /* token has more than 5 records */
				{
					fprintf(openfile,"\n");
					count=0;
				}
			}
			
			file_temp=file_temp->next;
		}
		
		fprintf(openfile,"\n</list>\n");
		name_temp=name_temp->next;
	}
	
	fclose(openfile);
	
	return 0;
}
int main(int argc, char **argv)
{
	if(argc!=3)
	{
		printf("Error: Invalid amount of arguments.\n");
		return 0;
	}
	
	/* Don't let over-writing occur */
	
	if(strcmp(argv[1],argv[2])==0)
	{
		fprintf(stderr,"Warning: Do not overwrite the existing file with your inverted index. Try again with a different output name. \n");
		return 0;
	}
	
	/* Initialize my linked list */
	
	inverted_list= SLCreate(compareWords);
	
	/* Traverse through the directory/files and build up the lists */
	
	process(argv[2]);
	
	/* Write your results to the inverted index file */
	
	writefile(inverted_list,argv[1]);
	
	/* Free all your allocated information from word and file lists. */
	
	SLDestroy(inverted_list);
	
	return 0;
}
