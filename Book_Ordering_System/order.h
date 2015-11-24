#ifndef ORDER_H
#define ORDER_H

/*
 * order.h
 */
 
 #include "queue.h"
 
 /* Represents the receipt generated from a successful/failed purchase */
 
 typedef struct receipt
 {
	 char *book_title;
	 double book_price;
	 double remain_credit; /* remaining credit after successful purchase */
	 struct receipt *next;
 } Receipt;
 
 struct ReceiptList
 {
	 struct receipt *head;
 };
 
 typedef struct ReceiptList* Receipts;
 
 /*repreents a customer in the database.txt file */
 
 typedef struct Customer
{
	char *full_name;
	int id_number;
	double credit_limit;
	char *street_address;
	char *state;
	char *zip_code;
	Receipts good; /* successful orders */
	Receipts bad; /* unsuccessful orders */
	struct Customer *next;
	
} Customer; 

struct CustomerList
{
	struct Customer *head;
};

typedef struct CustomerList* Database;

/* represents a single order from orders.txt */

typedef struct Order
{
	 char *title;
	 double price;
	 int id;
	 char *category;
	 struct Order *next;  
} order;
 
 struct OrderList
 {
	 struct Order *head;
 };
 
 typedef struct OrderList* Orders;
 
 Customer *findperson(int id, Database db);
 Receipt *createStub(char *title, double price, double credit);
 int insertReceipt(Receipts list, Receipt *item);
 
#endif
