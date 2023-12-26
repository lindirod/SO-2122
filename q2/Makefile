PROG = addmx
OBJS = addmx.o
SOURCE = addmx.c
CC = gcc
CFLAGS = -g -c -Wall 


all: $(OBJS)
		$(CC) -g $(OBJS) -o $(PROG)

addmx.o: addmx.c
	$(CC) $(CFLAGS) addmx.c 


clean:
	rm -f $(PROG)
	rm -f $(OBJS)
