#include "monitor_signal_continue.h"
#define NUM_PROD_H 1
#define NUM_PROD_L 3
#define NUM_CONS 1
#define DIM 3

typedef struct {
    int buffer[DIM];
    int testa; 
    int coda;
    int riemp;
    Monitor m;
    
} PriorityProdCons;

#define VAR_COND_PRODH  0
#define VAR_COND_PRODL  1
#define VAR_COND_CONS   2

void inizializza_prod_cons(PriorityProdCons * p);
void produci_alta_prio(PriorityProdCons * p);
void produci_bassa_prio(PriorityProdCons * p);
void consuma(PriorityProdCons * p);
void rimuovi_prod_cons(PriorityProdCons * p);