/*
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ 
{
	char *next_sep_ptr; /* next separator pointer */
	char *next_token_ptr; /* next token pointer */
	char *created_token; /*created token from function */
	int counter; /* array index of next starting point */
};

typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given set of separator
 * characters (given as a string) and a token stream (given as a string).
 * 
 * TKCreate should copy the two arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate(char *separators, char *ts) 
{
	int sep_length=0;
	int total_sep_length=0;
	int total_stream_length=0;
	
	if(separators!=NULL)
	{
		sep_length=strlen(separators);
	}
	
	total_sep_length= sep_length+1; /* make space for \0 */
	int stream_length=0;
	
	if(ts!=NULL)
	{
		stream_length=strlen(ts);
	}
	
	total_stream_length= stream_length+1; /* make space for \0 */
	
	TokenizerT *token= (TokenizerT *)malloc(sizeof(TokenizerT));
	token->next_token_ptr=(char *)malloc(sizeof(char)*total_stream_length); /*allocate space for token stream */
	token->next_sep_ptr=(char *)malloc(sizeof(char)*total_sep_length); /*allocate space for separator stream */
	
	strcpy(token->next_token_ptr,ts); /* copy over the token stream */
	strcpy(token->next_sep_ptr, separators); /* copy over the separator stream */
	
  return token;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy(TokenizerT *tk) 
{
	/* free up all the parameters that were malloced */
	
	free(tk->next_sep_ptr);
	free(tk->next_token_ptr);
	free(tk);
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken(TokenizerT *tk) 
{
	/* returning token is overall->created_token. I am checking
	 * character by character for separators. Once I encounter a separator, I allocate space
	 * in an array by my counter, and then malloc created_token. Then I strcpy the tokenized
	 * string and return created_token. Maintaining control of memory is essential. */
	
	int count=0;
	int i=tk->counter; /* traverse token stream */
	int flag=0;
	int j=0; /* traverse separator stream */
	
	while (tk->next_token_ptr[i]!='\0')
	{
		j=0; /* reset the separator stream back to */
		
		while(tk->next_sep_ptr[j]!='\0')
		{
			if(tk->next_token_ptr[i]==tk->next_sep_ptr[j])
			{
				/*you encountered a separator. There are several cases that need to
				 * be taken care of, which include escape character separators. Escape Characters
				 * are appended to the created_token in order for it to be handled in the main
				 * printing process. Non-escape characters are normally flagged as separators.*/
				 
				 if(tk->next_token_ptr[i]=='"' && tk->next_sep_ptr[j]=='"')
				 {
					 flag=0;
				 }
				 
				 if(tk->next_token_ptr[i]=='\\' && tk->next_sep_ptr[j]=='\\')
				 {
					if(tk->next_sep_ptr[j+1]!='\0' && tk->next_token_ptr[i+1]!='\0')
					{
						if(tk->next_sep_ptr[j+1]=='n' && tk->next_token_ptr[i+1]=='n')
						{
							flag=0;
						}
						
						if(tk->next_sep_ptr[j+1]=='t' && tk->next_token_ptr[i+1]=='t')
						{
							flag=0;
						}
						
						if(tk->next_sep_ptr[j+1]=='v' && tk->next_token_ptr[i+1]=='v')
						{
							flag=0;
						}
						
						if(tk->next_sep_ptr[j+1]=='b' && tk->next_token_ptr[i+1]=='b')
						{
							flag=0;
						}
						
						if(tk->next_sep_ptr[j+1]=='n' && tk->next_token_ptr[i+1]=='r')
						{
							flag=0;
						}
						
						if(tk->next_sep_ptr[j+1]=='n' && tk->next_token_ptr[i+1]=='f')
						{
							flag=0;
						}
						
						if(tk->next_sep_ptr[j+1]=='a' && tk->next_token_ptr[i+1]=='a')
						{
							flag=0;
						}
						
						if(tk->next_sep_ptr[j+1]=='\\' && tk->next_token_ptr[i+1]=='\\')
						{
							flag=0;
						}
					}
				 }
				 else if(tk->next_token_ptr[i]=='n'|| tk->next_token_ptr[i]=='t' ||tk->next_token_ptr[i]=='v'||tk->next_token_ptr[i]=='b'||tk->next_token_ptr[i]=='r'|| tk->next_token_ptr[i]=='f'|| tk->next_token_ptr[i]=='a')
				 {
					 if (tk->next_sep_ptr[j-1]=='\\' && tk->next_token_ptr[i-1]=='\\')
					 {
							 flag=0;
					 }
					 else if(tk->next_sep_ptr[j-1]!='\\' && tk->next_token_ptr[i-1]!='\\')
					 {
						 
						 flag=1;
					 }
				 }
				 else
				 {
					 if(tk->next_token_ptr[i]!='"')
					 {
						flag=1;
					}
				 }	
			}
			
			j++;
		}
		
		if(flag==1)
		{
			break; /*stop the stream once a separator was found */
		}
		
		count++; /* this equals the total amount of characters to be inserted
				 into the stream */
		i++;
	}
	
	if(count==0) /* if no characters are found before a separator, the string must be discarded */
	{
		tk->counter++;
		return NULL;
	}
	
	int total_chars= count+1; /* factor in the \0 */
	
	tk->created_token= (char *)malloc(sizeof(char)*total_chars);
	
	char swap_array[total_chars];
	
	int traverse=0;
	int holder=tk->counter;
	
	while (traverse<count)
	{	
		/*transferring the non-sep characters into returned token */
		
		swap_array[traverse]=tk->next_token_ptr[holder];
		holder++; 	
		traverse++;
	}
	
	tk->counter=holder;
	swap_array[total_chars-1]='\0';
	
	strcpy(tk->created_token, swap_array);
	
	return tk->created_token;
}

/*
 * main will have two string arguments (in argv[1] and argv[2]).
 * The first string contains the separator characters.
 * The second string contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) 
{
	char newline[7]="[0x0a]";
	char hor_tab[7]="[0x09]";
	char ver_tab[7]="[0x0b]";
	char backspace[7]= "[0x08]";
	char carriage[7]= "[0x0d]";
	char form[7]= "[0x0c]";
	char audible[7]="[0x07]";
	char backslash[7]= "[0x5c]";
	char double_quote[7]= "[0x22]";
	
	if(argc<3)
	{
		fprintf(stderr, "Error: Invalid Amount of Arguments.\n");
		return 0;
	}
	
	TokenizerT *overall= TKCreate(argv[1],argv[2]);
	overall->counter=0;
	int i=0;
	int j=0;
	int sep_flag=0;
	int print_flag=0;
	
	/*traverse through the token stream and create tokens out of them 
	 * Separators and Hex Modifications are handled within the bottom loop.
	 * The returning token only has non-escape characters that acted as a separator*/
	
	while(overall->counter<strlen(overall->next_token_ptr))
	{
		char *token= TKGetNextToken(overall);
		i=0;
		
		if(token==NULL)
		{
			continue; /* this occurs when a single separator was found */
		}
		print_flag=0;
		
		if(strlen(token)==2)    
		{
			/*when the token string is just an escape character
			* acting as a separator */
	
			j=0;
			int tester=0;
			
			if(token[i]=='\\')
			{
				while(overall->next_sep_ptr[j]!='\0')
				{
					if(token[i+1]!='\0')
					{
						if(overall->next_sep_ptr[j]==token[i+1])
						{
							tester=1;
						}
					}
					
					j++;
				}
				
				if(tester==1)
				{
					free(token);
					continue;
				}
			}
		}
		while(token[i]!='\0')
		{
			j=0;
			sep_flag=0;
			
			/* Each of the 9 escape characters are individually handled below to 
			 * print either hex, ignore, or split as a separator. */
			 
			if(token[i]=='n')
			{
				if(i!=0)
				{
					if(token[i-1]=='\\')
					{
						while(overall->next_sep_ptr[j]!='\0')
						{
							if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='n')
							{
								sep_flag=1;
							}
									
							j++;
						}
						
						if(sep_flag==0)
						{
							printf("%s",newline);
							print_flag=1;
							i++;
							continue;	
						}
					}
				}
			}
			if(token[i]=='t')
			{
				if(i!=0)
				{
					if(token[i-1]=='\\')
					{
						while(overall->next_sep_ptr[j]!='\0')
						{
							if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='t')
							{
								sep_flag=1;
							}
									
							j++;
						}
						
						if(sep_flag==0)
						{
							printf("%s",hor_tab);
							print_flag=1;
							i++;
							continue;	
						}
					}
				}
			}
			if(token[i]=='v')
			{
				if(i!=0)
				{
					if(token[i-1]=='\\')
					{
						while(overall->next_sep_ptr[j]!='\0')
						{
							if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='v')
							{
								sep_flag=1;
							}
									
							j++;
						}
						
						if(sep_flag==0)
						{
							printf("%s",ver_tab);
							print_flag=1;
							i++;
							continue;	
						}
					}
				}
			}
			if(token[i]=='b')
			{
				if(i!=0)
				{
					if(token[i-1]=='\\')
					{
						while(overall->next_sep_ptr[j]!='\0')
						{
							if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='b')
							{
								sep_flag=1;
							}
									
							j++;
						}
						
						if(sep_flag==0)
						{
							printf("%s",backspace);
							print_flag=1;
							i++;
							continue;	
						}
					}
				}
			}
			if(token[i]=='r')
			{
				if(i!=0)
				{
					if(token[i-1]=='\\')
					{
						while(overall->next_sep_ptr[j]!='\0')
						{
							if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='r')
							{
								sep_flag=1;
							}
									
							j++;
						}
						
						if(sep_flag==0)
						{
							printf("%s",carriage);
							print_flag=1;
							i++;
							continue;	
						}
					}
				}
			}
			if(token[i]=='f')
			{
				if(i!=0)
				{
					if(token[i-1]=='\\')
					{
						while(overall->next_sep_ptr[j]!='\0')
						{
							if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='f')
							{
								sep_flag=1;
							}
									
							j++;
						}
						
						if(sep_flag==0)
						{
							printf("%s",form);
							print_flag=1;
							i++;
							continue;	
						}
					}
				}
			}
			if(token[i]=='a')
			{
				if(i!=0)
				{
					if(token[i-1]=='\\')
					{
						while(overall->next_sep_ptr[j]!='\0')
						{
							if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='a')
							{
								sep_flag=1;
							}
									
							j++;
						}
						
						if(sep_flag==0)
						{
							printf("%s",audible);
							print_flag=1;
							i++;
							continue;	
						}
					}
				}
			}
			if(token[i]=='"')
			{
				while(overall->next_sep_ptr[j]!='\0')
				{
					if(overall->next_sep_ptr[j]=='"')
					{
						sep_flag=1;
					}
									
					j++;
				}
						
				if(sep_flag==0)
				{
					printf("%s",double_quote);
					print_flag=1;
					i++;
					continue;	
				}	
			}
			if(token[i]=='\\')
			{
				if(token[i+1]=='\\')
				{
					while(overall->next_sep_ptr[j]!='\0')
					{
						if(overall->next_sep_ptr[j]=='\\' && overall->next_sep_ptr[j+1]=='\\')
						{
							sep_flag=1;
						}
									
						j++;
					}
						
					if(sep_flag==0)
					{
						printf("%s",backslash);
						print_flag=1;
						i++;
						continue;	
					}
				}
			}
			
			if(sep_flag!=1 && token[i]!='\\')
			{
				/* a normal character being printed */
				
				printf("%c",token[i]);
				print_flag=1;
			}
			
			else if(sep_flag==1 && i!=strlen(token)-1)
			{
				/*checking to see if the next character is a separator. If so, it needs
				 * to be skipped. */
				 
				if(token[i+1]!='\0' && token[i+2]!='\0')
				{
					if(token[i+1]=='\\')
					{
						if(token[i+2]=='n' || token[i+2]=='t'||token[i+2]=='v'||token[i+2]=='b'||token[i+2]=='r'||token[i+2]=='f'||token[i+2]=='a'|| token[i+2]=='"' ||token[i+2]=='\\')
						{
							j=0;
							int checker=0;
							
							while(overall->next_sep_ptr[j]!='\0')
							{
								if(overall->next_sep_ptr[j]=='\\')
								{
									if(overall->next_sep_ptr[j+1]!='\0')
									{
										if(overall->next_sep_ptr[j+1]==token[i+2])
										{
											checker=1;
										}
									}
								}
								j++;
							}
							if(checker==1)
							{
								i++;
								continue;
							}
							else
							{
								if(print_flag==1)
								{
									printf("\n");
									i++;
									continue;
								}
								else
								{
									i++;
									continue;
								}
							}
						}
					}
				}
				if(i!=1 && print_flag!=0)
				{
					printf("\n");	
				}
				
			}
			
			i++;
		}
		
		if(strlen(token)==1 && token[0]=='\\')
		{
			free(token);
			continue;
		}
		
		printf("\n");
		free(token);		
	}
	
	TKDestroy(overall);
	
	return 0;
}
