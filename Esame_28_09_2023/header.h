#include <pthread.h>

#ifndef __HEADER
#define __HEADER

#define DIM_BUFFER 5

#define NUM_REQ 10

//definizione eventuale di altre costanti
#define MUTEXP 0
#define MUTEXC 1
#define SPAZIO_DISP 2
#define MSG_DISP 3

struct Coda {
    int buffer[DIM_BUFFER];
    int testa;
    int coda;
};

void produci(struct Coda *, int id_sem, int num_prenotazioni);
int consuma(struct Coda *, int id_sem);

// struttura Count con num_prenotazioni condivisa lato server
typedef struct{
	int num_prenotazioni;

    pthread_mutex_t mutex;
}Count;

#endif
