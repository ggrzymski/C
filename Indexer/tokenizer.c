#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"

#define MAX_HEX_CHARS 2
#define MAX_OCT_CHARS 3

/* I am creating a Tokenizer object with a File stream .  */
 
TokenizerT *TKCreate(char *file) 
{
	/* make sure input is valid */
	
	if(file==NULL)
	{
		return NULL;
	}
	
	TokenizerT *file_search= (TokenizerT*)malloc(sizeof(TokenizerT));
	
	if(file_search==NULL)
	{
		return NULL;
	}
	
	/* I need to set the File stream for reading purposes only. */
	
	FILE *stream= fopen(file,"r");
	
	/* Make sure the file is opened right */
	
	if(stream==NULL)
	{
		fprintf(stderr, "File wasn't read properly.\n");
		return NULL;
	}
	
	file_search->file_name= stream;
	
	return file_search;
}

/* The tokenizer object is destroyed by having its file stream closed, and
 * also freeing the malloced memory of the Tokenizer struct. */
 
void TKDestroy(TokenizerT *tk) 
{	
	if(tk==NULL)
	{
		return;
	}
	
	/* "Destroy" the file by closing it */
	
	int test= fclose(tk->file_name);
	
	if(test!=0)
	{
		fprintf(stderr,"File wasn't closed properly.\n");
		return;
	}
	
	free(tk);
}

char *TKGetNextToken(TokenizerT *tk) 
{
	int index;
	
	fpos_t orig_pos; /*tracks the position of my file */
	
	int token_length=0; /* for mallocing properly */
	
	char *token;
	
	/* keep track of the position while traversing */
	
	fgetpos(tk->file_name, &orig_pos);
	
	/* First, make sure there is a tokenizable object in the stream. Find the first
	 * alphanumeric character in the stream */
	 
	while((index=fgetc(tk->file_name))!=EOF)
	{
		if(isalnum(index))
		{
			token_length++;
			break;
		}
		
		fgetpos(tk->file_name, &orig_pos);
	}
	
	/* no tokens to process */
	
	if(index==EOF)
	{
		return NULL;
	}
	
	while(index!=EOF)
	{
		index=fgetc(tk->file_name);
		
		if(!isalnum(index)) /* stop the stream */
		{
			break;
		}
		
		token_length++;
	}
	
	/* Create the token now */
	
	token=(char *)malloc(sizeof(char*)*(token_length+1)); /* need to account for \0 */
	
	/* Get the stream back to the beginning of the first alphanumeric character */
	
	fsetpos(tk->file_name, &orig_pos);
	
	/* get the token. Fgets automatically gives me the null character, so
	 * I need to compensate that by adding 1 to make sure last character isn't
	 * cut off. */
	
	fgets(token, token_length+1, tk->file_name);
	
	/* Everything needs to be lower case. Use ctype islower */
	
	int traverse;
	
	for(traverse=0; traverse<strlen(token); traverse++)
	{
		token[traverse]= tolower(token[traverse]);
	}
	
	return token;
}
