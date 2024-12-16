all: client server main

client: client.c header.h
	gcc -o client client.c

server: server.c header.h
	gcc -o server server.c -pthread

main: main.c
	gcc -o main main.c

clean:
	rm client server main