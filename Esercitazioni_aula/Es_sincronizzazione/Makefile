all: es_sincronizzazione

es_sincronizzazione: es_sincronizzazione.o semafori.o
	gcc -o es_sincronizzazione es_sincronizzazione.o semafori.o

es_sincronizzazione.o: es_sincronizzazione.c semafori.c
	gcc -c es_sincronizzazione.c -o es_sincronizzazione.o

semafori.o: semafori.c
	gcc -c semafori.c -o semafori.o

clean:
	rm -f *.o
	rm -f es_sincronizzazione