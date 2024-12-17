all: main client_asta server_asta

main: main.c
	gcc -o main main.c

client_asta: client_asta.c header.h
	gcc -o client_asta client_asta.c

server_asta: server_asta.c header.h
	gcc -o server_asta server_asta.c

clean:
	rm client_asta
	rm server_asta
	rm main