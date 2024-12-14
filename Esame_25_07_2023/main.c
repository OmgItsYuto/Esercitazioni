#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "procedure.h"

#define QUANTITA_INIZIALE 500
#define VALORE_INIZIALE 100

void * aggiornatore(void *);
void * azionista(void *);

int main() {

    pthread_t th_upd,th_act[NUM_AZIONISTI];

    MonitorPortafoglio *m[NUM_AZIONISTI];

    for(int i=0;i<3;i++) {
        /* Creazione ed inizializzazione degli oggetti monitor */
        m[i]=(MonitorPortafoglio *)malloc(sizeof(MonitorPortafoglio));
        inizializza(m[i],500,100);
    }

    /* TBD: Creare il thread aggiornatore, passargli il vettore di oggetti monitor */
    pthread_create(&th_upd,NULL,aggiornatore,(void *)m);

    for(int i=0;i<NUM_AZIONISTI;i++) {
        /* TBD: Creare i thread azionisti, passando a ognuno uno degli oggetti monitor */
        pthread_create(&th_act[i],NULL,azionista,(void *)m[i]);
    }

    /* TBD: Attendere la terminazione del thread aggiornatore */
    pthread_join(th_upd,NULL);

    for(int i=0;i<NUM_AZIONISTI;i++) {
        /* TBD: Attendere la terminazione dei thread azionisti, deallocare gli oggetti */
        pthread_join(th_act[i],NULL);
    }

    //rimozione monitors e deallocazione memoria
    for(int i=0;i<NUM_AZIONISTI;i++){
        termina(m[i]);
        free(m[i]);
    }

    pthread_exit(0);
}

void * aggiornatore(void *par) {

    /* TBD: Gestire il passaggio dei parametri

        Suggerimento: Dichiarare l'array di oggetti con la sintassi "MonitorPortafoglio ** m = ..." 
    */

    MonitorPortafoglio **m=(MonitorPortafoglio **)par;

    for(int i=0;i<10;i++) {

        for(int j=0;j<3;j++) {

            int valore=100+((rand()%21)-10);

            printf("[AGGIORNATORE] Aggiornamento titolo %d, valore %d\n", j, valore);

            /* TBD: Invocare il metodo "aggiorna" sull'oggetto monitor */
            aggiorna(m[j],valore);
        }

        sleep(1);
    }

    printf("[AGGIORNATORE] Terminazione\n");

    for(int j=0; j<3; j++) {

        /* TBD: Invocare il metodo "aggiorna" sull'oggetto monitor, con valore 200 */
        aggiorna(m[j],200);

        sleep(1);

        /* TBD: Invocare il metodo "aggiorna" sull'oggetto monitor, con valore 0 */
        aggiorna(m[j],0);
    }

    pthread_exit(0);
}


void * azionista(void *par) {

    /* TBD: Gestire il passaggio dei parametri */
    MonitorPortafoglio *m=(MonitorPortafoglio *)par;

    printf("[AZIONISTA] Richiesta di vendita azioni, prezzo offerto 105\n");

    /* TBD: Invocare il metodo "vendita" sull'oggetto monitor, con valore 105 e quantità 50 */
    vendita(m,50,105);

    int valore=leggi(m);

    printf("[AZIONISTA] Vendita effettuata, valore attuale %d\n", valore);


    printf("[AZIONISTA] Richiesta di acquisto azioni, prezzo offerto 95\n");

    /* TBD: Invocare il metodo "acquisto" sull'oggetto monitor, con valore 95 e quantità 50 */
    acquisto(m,50,95);

    valore=leggi(m);

    printf("[AZIONISTA] Acquisto effettuato, valore attuale %d\n", valore);


    printf("[AZIONISTA] Terminazione\n");

    pthread_exit(0);    
}