all: main server client

client: client.o
	gcc -o client client.o

server: server.o
	gcc -o server server.o

main: main.o
	gcc -o main main.o

client.o: client.c header.h
	gcc -o client.o -c client.c

server.o: server.c header.h
	gcc -o server.o -c server.c

main.o: main.c header.h
	gcc -o main.o -c main.c

clean:
	rm -f main server client
	rm -f *.o