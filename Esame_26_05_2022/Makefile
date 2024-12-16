all: main

main: main.o prod_cons.o monitor_hoare.o
	gcc -o main main.o prod_cons.o monitor_hoare.o

main.o: main.c prod_cons.h
	gcc -o main.o -c main.c

monitor_hoare.o: monitor_hoare.c monitor_hoare.h
	gcc -o monitor_hoare.o -c monitor_hoare.c

prod_cons.o: prod_cons.c prod_cons.h
	gcc -o prod_cons.o -c prod_cons.c

clean:
	rm -f *.o
	rm -f main