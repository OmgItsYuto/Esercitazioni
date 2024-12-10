#include <pthread.h>

#define NUMDVD 6
#define NUMCLIENTI 4
#define NUMPRINTERS 1

#define DISPONIBILE 0
#define AFFITTATO 1

struct DVD {
    int identificativo_film;
    // un intero tra 1 e 3
    int identificativo_copia;
    // un intero tra 1 e 2
    int stato;
    // DISPONIBILE=0, oppure AFFITTATO=1
};
struct Monitor {
    struct DVD dvd[NUMDVD];    // vettore di DVD da gestire
    
    // aggiungere qui variabili per la sincronizzazione

    int disp_cnt[3];    //vettore di disponibilità posizionale

    pthread_mutex_t mutex;
    /*pthread_cond_t cond_affitto_film1;
    pthread_cond_t cond_affitto_film2;
    pthread_cond_t cond_affitto_film3;
    */
   pthread_cond_t cond_affitto[3];
};

int affitta(struct Monitor * m, int id_film);
void restituisci(struct Monitor * m, int id_film, int id_copia);
void stampa(struct Monitor * m);

