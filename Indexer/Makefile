COMPILER = gcc
CCFLAGS = -g -Wall
all: index

debug: 
	make DEBUG=TRUE
	
index: index.o libsl.a tokenizer.o 
	$(COMPILER) $(CCFLAGS) -o index index.o libsl.a tokenizer.o
tokenizer.o: tokenizer.c tokenizer.h
	$(COMPILER) $(CCFLAGS) -c tokenizer.c
index.o: index.c index.h
	$(COMPILER) $(CCFLAGS) -c index.c
libsl.a: sorted-list.o
	ar rvs libsl.a sorted-list.o
sorted-list.o: sorted-list.c sorted-list.h
	$(COMPILER) $(CCFLAGS) -c sorted-list.c

ifeq ($(DEBUG), TRUE)
 CCFLAGS +=-g
endif

clean:
	rm -f index
	rm -f *.o
	rm -f libsl.a
