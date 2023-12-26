PROG = phrases
OBJS = phrases.o
SOURCE = phrases.c
CC = gcc
CFLAGS = -g -c -Wall 

all: $(OBJS)
		$(CC) -g $(OBJS) -o $(PROG) 


phrases.o: phrases.c
	$(CC) $(CFLAGS) phrases.c 


clean:
	rm -f $(PROG)
	rm -f $(OBJS)
