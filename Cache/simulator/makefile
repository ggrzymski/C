COMPILER = gcc
CCFLAGS = -ansi -lm -pedantic -Wall -Wextra -std=c99
all: cache-sim

debug: 
	make DEBUG=TRUE

cache-sim: sim.o
	$(COMPILER) $(CCFLAGS) -o cache-sim sim.o
sim.o: sim.c
	$(COMPILER) $(CCFLAGS) -c sim.c 

ifeq ($(DEBUG), TRUE)
 CCFLAGS +=-g
endif

clean:
	rm -f cache-sim
	rm -f *.o
