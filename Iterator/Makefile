COMPILER = gcc
CCFLAGS = -g -Wall
all: sl

debug: 
	make DEBUG=TRUE

sl: main.o libsl.a
	$(COMPILER) $(CCFLAGS) -o sl main.o libsl.a
sorted-list.o: sorted-list.c sorted-list.h
	$(COMPILER) $(CCFLAGS) -c sorted-list.c 
libsl.a: sorted-list.o
	ar rvs libsl.a sorted-list.o
main.o: main.c sorted-list.h
	$(COMPILER) $(CCFLAGS) -c main.c

ifeq ($(DEBUG), TRUE)
 CCFLAGS +=-g
endif

clean:
	rm -f sl
	rm -f *.o
	rm -f libsl.a
