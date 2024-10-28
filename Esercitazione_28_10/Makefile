all: start produttore consumatore

start: procedure.o start.o
	gcc -o start procedure.o start.o -lm

produttore:	produttore.o procedure.o
	gcc -o produttore produttore.o procedure.o -lm

produttore.o: produttore.c
	gcc -c produttore.c

consumatore: consumatore.o procedure.o
	gcc -o consumatore consumatore.o procedure.o -lm

consumatore.o: consumatore.c
	gcc -c consumatore.c

procedure.o: procedure.c
	gcc -c procedure.c 

clean:
	rm -rf *.o
	rm -rf start
	rm -rf consumatore
	rm -rf produttore