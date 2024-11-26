#ifndef HEADER_H
#define HEADER_H

#include "monitor_hoare.h"

#define N_GENERATORI 3
#define N_CALCOLATORI 3
#define N_PRINTERS 3

#define N_OPS 3

// TODO DEFINIRE MACRO DI SUPPORTO
#define MSG_SOMMA 1
#define MSG_PRODOTTO 2 
#define MSG_DIVISIONE 3

struct MonitorRisultati {
	float risultati[N_OPS];

	int numero_printers;
	int numero_calcolatori;

	Monitor m;
};

#define VARCOND_PRINTERS 0
#define VARCOND_CALCOLATORI 1

void inserisci_risultato(struct MonitorRisultati * ls, float valore, int operazione);
float* leggi_risultati(struct MonitorRisultati * ls);

typedef struct {
	long tipo;
	float valore;
} Messaggio;

void calcolo(struct MonitorRisultati * ls, int operazione, int mailbox);
void generazione(int operazione, int mailbox);
void printer(struct MonitorRisultati * ls);

#endif
