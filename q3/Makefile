PROG = cypher
OBJS = cypher.o
SOURCE = cypher.c
CC = gcc
CFLAGS = -g -c -Wall 


all: $(OBJS)
		$(CC) -g $(OBJS) -o $(PROG)

cypher.o: cypher.c
	$(CC) $(CFLAGS) cypher.c 


clean:
	rm -f $(PROG)
	rm -f $(OBJS)
