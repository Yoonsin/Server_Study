CFLAGS= -lpthread
CC= gcc

client_racing: client_racing.c keyboard.o queue.o
	$(CC) -o client_racing client_racing.c keyboard.o queue.o $(CFLAGS)


clean:
	rm -f client_racing keyboard.o queue.o

rebuild: clean all

keyboard.o: keyboard.c keyboard.h
	$(CC) $(CFLAGS) -c keyboard.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c
