all: prodcons_singolo_buffer produttore consumatore

prodcons_singolo_buffer: procedure.o semafori.o prodcons_singolo_buffer.o
	gcc -o prodcons_singolo_buffer procedure.o semafori.o prodcons_singolo_buffer.o

produttore:	produttore.o procedure.o semafori.o
	gcc -o produttore produttore.o procedure.o semafori.o

produttore.o: produttore.c
	gcc -c produttore.c

consumatore:	consumatore.o procedure.o semafori.o
	gcc -o consumatore consumatore.o procedure.o semafori.o

consumatore.o: consumatore.c
	gcc -c consumatore.c

prodcons_singolo_buffer.o: prodcons_singolo_buffer.c
	gcc -c prodcons_singolo_buffer.c

procedure.o: procedure.h procedure.c
	gcc -c procedure.c

semafori.o: semafori.c semafori.h
	gcc -c semafori.c

clean:
	rm -rf *.o
	rm -rf prodcons_singolo_buffer
	rm -rf consumatore
	rm -rf produttore