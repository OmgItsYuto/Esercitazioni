#ifndef _PROCEDURE_
#define _PROCEDURE_

#include <pthread.h>

#define NUM_AZIONISTI 3

typedef struct {

    int valore;
    int quantita;

    /* TBD: Aggiungere ulteriori variabili per la sincronizzazione */
    pthread_cond_t cv_acquisto;
    pthread_cond_t cv_vendita;
    pthread_mutex_t mutex;

} MonitorPortafoglio;

void inizializza(MonitorPortafoglio * m, int quantita_iniziale, int valore_iniziale);
void termina(MonitorPortafoglio * m);

void acquisto(MonitorPortafoglio * m, int quantita, int prezzo);
void vendita(MonitorPortafoglio * m, int quantita, int prezzo); 
void aggiorna(MonitorPortafoglio * m, int valore);
int leggi(MonitorPortafoglio * m);

#endif