#include <stdio.h>
#include <unistd.h>

#include "prodcons.h"


void init_monitor(MonitorProdCons * p) {

    for(int i=0;i<DIM;i++)
        p->buffer[i]=0;

    p->coda=p->testa=p->riemp=0;

    /* TBD: Inizializzare l'oggetto monitor */
    pthread_cond_init(&(p->cond_ripresa),NULL);
    pthread_cond_init(&(p->cond_produzione),NULL);
    pthread_cond_init(&(p->cond_consumazione),NULL);
    pthread_mutex_init(&(p->mutex),NULL);

    p->conteggio_consumazioni = 0;
}

void produzione(MonitorProdCons * p, int val) {

    /* TBD: Completare la sincronizzazione, in base allo schema
            del vettore di buffer circolare */
    pthread_mutex_lock(&(p->mutex));

    while(p->riemp==DIM){
        pthread_cond_wait(&(p->cond_produzione),&(p->mutex));
    }
    
    p->buffer[p->coda]=val;
    p->coda=(p->coda+1)%DIM;
    p->riemp++;
    
    pthread_cond_signal(&(p->cond_consumazione));

    pthread_mutex_unlock(&(p->mutex));

}


int consumazione(MonitorProdCons * p) {

    int val;

    /* TBD: Completare la sincronizzazione, in base allo schema
            del vettore di buffer circolare */
    pthread_mutex_lock(&(p->mutex));

    while(p->riemp==0){
        pthread_cond_wait(&(p->cond_consumazione),&(p->mutex));
    }
    
    val=p->buffer[p->testa];
    p->testa=(p->testa+1)%DIM;
    p->riemp--;
    p->conteggio_consumazioni++;
    sleep(2);

    pthread_cond_signal(&(p->cond_ripresa));

    pthread_cond_signal(&(p->cond_produzione));

    pthread_mutex_unlock(&(p->mutex));

    return val;
}

int attendi_consumazioni(MonitorProdCons * p, int soglia) {

    int n;

    /* TBD: Sospendere il thread finché il "conteggio_consumazioni" non sia diventato
            maggiore o uguale a "soglia"
     */
    pthread_mutex_lock(&(p->mutex));

    while(p->conteggio_consumazioni<soglia){
        printf("[CONTEGGIO] Valore conteggio: %d\n",p->conteggio_consumazioni);
        printf("Mi metto in attesa sulla varcond ripresa\n");
        pthread_cond_wait(&(p->cond_ripresa),&(p->mutex));
    }

    n = p->conteggio_consumazioni;
    p->conteggio_consumazioni = 0;

    pthread_mutex_unlock(&(p->mutex));
    
    return n;
}

void remove_monitor(MonitorProdCons * p) {
    /* TBD: Disattivare le variabili per la sincronizzazione */
    pthread_cond_destroy(&(p->cond_ripresa));
    pthread_cond_destroy(&(p->cond_produzione));
    pthread_cond_destroy(&(p->cond_consumazione));
    pthread_mutex_destroy(&(p->mutex));
}