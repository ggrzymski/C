COMPILER = gcc
CCFLAGS = -g -Wall
all: bookorders

debug: 
	make DEBUG=TRUE
	
bookorders: bookorders.o queue.o order.o
	$(COMPILER) $(CCFLAGS) -o bookorders bookorders.o queue.o order.o -lpthread
bookorders.o: bookorders.c bookorders.h
	$(COMPILER) $(CCFLAGS) -c bookorders.c
queue.o: queue.c queue.h
	$(COMPILER) $(CCFLAGS) -c queue.c
order.o: order.c order.h
	$(COMPILER) $(CCFLAGS) -c order.c
	
ifeq ($(DEBUG), TRUE)
 CCFLAGS +=-g
endif

clean:
	rm -f bookorders
	rm -f *.o
