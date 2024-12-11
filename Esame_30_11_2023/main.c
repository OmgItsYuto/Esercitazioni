#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prodcons.h"

#define NUM_THREAD_PROD 3
#define NUM_THREAD_CONS 3
#define NUM_PRODUZIONI 10
#define NUM_CONSUMAZIONI 10
#define CONTEGGIO 5

void * thread_produttore(void *);
void * thread_consumatore(void *);
void * thread_conteggio(void *);


int main() {

    pthread_t th_p[NUM_THREAD_PROD];
    pthread_t th_c[NUM_THREAD_CONS];
    pthread_t th_cnt;

    /* TBD: Creare un oggetto monitor di tipo "MonitorProdCons" */
    MonitorProdCons *monitor=(MonitorProdCons *)malloc(sizeof(MonitorProdCons));
    
    init_monitor(monitor);

    for(int i=0; i<NUM_THREAD_PROD; i++) {
        /* TBD: Creare un thread produttore, fargli eseguire la funzione "thread_produttore" */
        pthread_create(&th_p[i],NULL,thread_produttore,(void*)monitor);
    }

    for(int i=0; i<NUM_THREAD_CONS; i++) {
        /* TBD: Creare un thread consumatore, fargli eseguire la funzione "thread_consumatore" */
        pthread_create(&th_c[i],NULL,thread_consumatore,(void*)monitor);
    }


    /* TBD: Creare un thread di "conteggio", fargli eseguire la funzione "thread_conteggio" */
    pthread_create(&th_cnt,NULL,thread_conteggio,(void*)monitor);


    /* TBD: Attendere la terminazione dei thread */
    for(int i=0; i<NUM_THREAD_PROD; i++) {
        /* TBD: Creare un thread produttore, fargli eseguire la funzione "thread_produttore" */
        pthread_join(th_p[i],NULL);
    }
    for(int i=0; i<NUM_THREAD_CONS; i++) {
        /* TBD: Creare un thread produttore, fargli eseguire la funzione "thread_produttore" */
        pthread_join(th_c[i],NULL);
    }

    pthread_join(th_cnt,NULL);

    /* TBD: Deallocare l'oggetto monitor */
    remove_monitor(monitor);
    free(monitor);

    pthread_exit(0);
}

void * thread_produttore(void * x) {

    /* TBD: Completare il passaggio dell'oggetto "MonitorProdCons" */
    MonitorProdCons *p=(MonitorProdCons *)x;

    for(int i=0; i<NUM_PRODUZIONI; i++) {

        /* TBD: Completare la chiamata a produzione() */
        int val = rand() % 10;
        sleep(rand()%5);
        produzione(p, val);

        printf("[PRODUTTORE] Ho prodotto: %d\n", val);
    }

    pthread_exit(0);
}

void * thread_consumatore(void * x) {

    /* TBD: Completare il passaggio dell'oggetto "MonitorProdCons" */
    MonitorProdCons *p=(MonitorProdCons *)x;

    for(int i=0; i<NUM_CONSUMAZIONI; i++) {

        /* TBD: Completare la chiamata a consumazione() */
        int val = consumazione(p);

        printf("[CONSUMATORE] Ho consumato: %d\n", val);
    }

    pthread_exit(0);
}

void * thread_conteggio(void * x) {

    /* TBD: Completare il passaggio dell'oggetto "MonitorProdCons" */
    MonitorProdCons *p=(MonitorProdCons *)x;

    int consumazioni_effettuate = 0;

    int consumazioni_da_effettuare = NUM_CONSUMAZIONI * NUM_THREAD_CONS;

    while(consumazioni_da_effettuare > 0) {

        int conteggio;

        if( consumazioni_da_effettuare >= CONTEGGIO ) {

            // Vi sono 5 o più consumazioni ancora da effettuare,
            // si pone a 5 il conteggio da attendere
            conteggio = CONTEGGIO;
        }
        else {

            // Vi sono meno di 5 consumazioni ancora da effettuare,
            // si pone il conteggio da attendere a "consumazioni_da_effettuare"
            conteggio = consumazioni_da_effettuare;
        }

        /* TBD: Completare la chiamata ad "attendi_consumazioni()" */
        int n = attendi_consumazioni(p, conteggio);

        consumazioni_effettuate += n;

        consumazioni_da_effettuare -= n;

        printf("[CONTEGGIO] Sono stati consumati in totale %d elementi\n", consumazioni_effettuate);
    }

    pthread_exit(0);
}
