#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "header.h"


void init_monitor(MonitorIO * m) {

    /* Inizializzazione oggetto monitor */
    for(int i=0;i<DIM;i++)
        m->stato[i]=LIBERO;

    for(int i=0;i<DIM;i++)
        m->vettore[i]=0;

    m->num_liberi=DIM;
    m->num_occupati=0;

    pthread_cond_init(&(m->cv_attesa),NULL);
    pthread_cond_init(&(m->cv_cons),NULL);
    pthread_cond_init(&(m->cv_prod),NULL);
    
    pthread_mutex_init(&(m->mutex),NULL);

}

void end_monitor(MonitorIO * m) {

    /* Disattivazione delle variabili membro dell'oggetto monitor */
    pthread_cond_destroy(&(m->cv_prod));
    pthread_cond_destroy(&(m->cv_attesa));
    pthread_cond_destroy(&(m->cv_cons));

    pthread_mutex_destroy(&(m->mutex));

}

void produzione(MonitorIO * m, char c) {
    int i=0;

    /* TBD: Porre il thread in sospensione se il vettore di buffer è pieno */
    pthread_mutex_lock(&(m->mutex));

    while(m->num_liberi==0)
        pthread_cond_wait(&(m->cv_prod),&(m->mutex));

    /* TBD: Ricercare un elemento "i" nello stato LIBERO,
            in base allo schema con vettore di stato       */
    while(i<DIM && m->stato[i]!=LIBERO)
        i++;
    
    m->stato[i]=IN_USO;
    m->num_liberi--;

    pthread_mutex_unlock(&(m->mutex));

    printf("PRODUZIONE: inizio produzione vettore[%d]\n", i);
    sleep(1 + (rand() % 3));
    m->vettore[i] = c;
    printf("PRODUZIONE: vettore[%d] <- '%c'\n", i, c);
    m->stato[i]=OCCUPATO;

    pthread_mutex_lock(&(m->mutex));
    m->num_occupati++;
    pthread_mutex_unlock(&(m->mutex));
    
    /* TBD: Riattivare un thread consumatore sospeso */
    pthread_cond_signal(&(m->cv_cons));

    /* TBD: Riattivare i thread sospesi sulla variabile condition "cv_attesa" */
    pthread_cond_signal(&(m->cv_attesa));
}

char consumazione(MonitorIO * m) {

    int i=0;

    /* TBD: Porre il thread in sospensione se il vettore di buffer è vuoto */
    pthread_mutex_lock(&(m->mutex));

    while(m->num_occupati==0)
        pthread_cond_wait(&(m->cv_cons),&(m->mutex));

    /* TBD: Ricercare un elemento "i" nello stato OCCUPATO,
            in base allo schema con vettore di stato       */
    while(i<DIM && m->stato[i]!=OCCUPATO)
        i++;

    m->stato[i]=IN_USO;
   
    pthread_mutex_unlock(&(m->mutex));

    printf("CONSUMAZIONE: inizio consumazione vettore[%d]\n", i);
    sleep(1);
    char c = m->vettore[i];
    printf("CONSUMAZIONE: vettore[%d] -> '%c'\n", i, c);
    m->stato[i]=LIBERO;

    pthread_mutex_lock(&(m->mutex));
    m->num_liberi++;
    m->num_occupati--;
    pthread_mutex_unlock(&(m->mutex));

    /* TBD: Riattivare un thread produttore sospeso */
    pthread_cond_signal(&(m->cv_prod));

    return c;
}

int attesa(MonitorIO * m) {

    int occupati;

    /* TBD: Se il numero di buffer occupati è minore di 3,
     *      sospendere il thread sulla condition variable "cv_attesa" */
    pthread_mutex_lock(&(m->mutex));

    while(m->num_occupati<3)
        pthread_cond_wait(&(m->cv_attesa),&(m->mutex));

    occupati=m->num_occupati;

    pthread_mutex_unlock(&(m->mutex));

    return occupati;
}
