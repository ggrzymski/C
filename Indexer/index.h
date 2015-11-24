#ifndef INDEX_H
#define INDEX_H
/*
 * index.h
 */

#include <stdlib.h>
#include <dirent.h>

int compareWords(void *p1, void *p2);
int compareInts(int p1, int p2);
void processdir(char *main, DIR *directory); /* read the contents of a directory */
int writefile(WordPtr list,char *output); /* write the linked list into the inverted text file */
int process(char * file); /* handle the argv[2] command */

#endif
