main: main.o
	gcc -pthread -o main main.o

main.o: main.c header.h
	gcc -c -o main.o main.c

clean:
	rm -f main
	rm -f *.o