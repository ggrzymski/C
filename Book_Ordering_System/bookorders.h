#ifndef BOOKORDERS_H
#define BOOKORDERS_H

/*
 * bookorders.h
 */
 
#include "order.h"
 
int buildCategories(char *filename);
void* producer(void *arg);
void* consumer(void *arg);
int generateDB(char *filename);
void DestroyDatabase(Database list);
void printDataStructure();
void destroyCategories(char **list);
void writeReport(Database users);
void DestroyReceipts(Receipts list);

#endif
