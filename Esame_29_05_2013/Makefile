all: main client server

server: server.o
	gcc -o server server.o

client: client.o
	gcc -o client client.o

main: main.o
	gcc -o main main.o

main.o: main.c header.h
	gcc -o main.o -c main.c

server.o: server.c header.h
	gcc -o server.o -c server.c

client.o: client.c header.h
	gcc -o client.o -c client.c

clean:
	rm -f main client server
	rm -f *.o
