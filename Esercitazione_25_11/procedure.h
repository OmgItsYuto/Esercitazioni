#ifndef HEADER_H
#define HEADER_H

#include <sys/types.h>
#include "monitor_hoare.h"

#define N_UTENTI 5
#define N 10
#define N_RICHIESTEPERUTENTE 5

typedef struct{
    unsigned int posizione;
    pid_t processo;
}richiesta;

typedef struct{
	richiesta elementi[N];
	int testa;
	int coda;

    int cnt;

    Monitor m;
} BufferCircolare;

#define VARCOND_UTENTI 0
#define VARCOND_SCHED 1

void utente(BufferCircolare *buf);
void schedulatore(BufferCircolare *buf, pid_t *disco);

#endif
