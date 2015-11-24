#ifndef TOKENIZER_H
#define TOKENIZER_H
/*
 * tokenizer.h
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */
 
struct TokenizerT_ 
{
	FILE* file_name; /* name of file to utilize library functions */		
};

typedef struct TokenizerT_ TokenizerT;

TokenizerT *TKCreate(char *file);

void TKDestroy(TokenizerT *tk);

char *TKGetNextToken(TokenizerT *tk);

#endif
