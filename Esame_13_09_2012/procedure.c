#include "header.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void inizializza_prod_cons(PriorityProdCons * p){
    init_monitor(&(p->m),3);

    for(int i=0;i<DIM;i++)
        p->buffer[i]=0;

    p->coda=p->testa=p->riemp=0;
}

void produci_alta_prio(PriorityProdCons * p){
    int value;

    enter_monitor(&(p->m));

    while(p->riemp==DIM){
        wait_condition(&(p->m),VAR_COND_PRODH);
    }

    value=rand()%12;
    p->buffer[p->coda]=value;
    p->coda=(p->coda+1)%DIM;
    p->riemp++;
    printf("[%d] - ProduttoreH: valore prodotto %d\n",getpid(),value);


    signal_condition(&(p->m),VAR_COND_CONS);

    leave_monitor(&(p->m));
}

void produci_bassa_prio(PriorityProdCons * p){
    int value;
    enter_monitor(&(p->m));

    while(p->riemp==DIM || queue_condition(&(p->m),VAR_COND_PRODH)>0){
        int num=queue_condition(&(p->m),VAR_COND_PRODH);
        if(num>0) printf("[%d] - ProduttoreL: mi metto in attesa perchè ci sono %d produttori H in attesa\n",getpid(),num);
        wait_condition(&(p->m),VAR_COND_PRODL);
    }

    value=13+rand()%12;
    p->buffer[p->coda]=value;
    p->coda=(p->coda+1)%DIM;
    p->riemp++;
    printf("[%d] - ProduttoreL: valore prodotto %d\n",getpid(),value);
    
    signal_condition(&(p->m),VAR_COND_CONS);

    leave_monitor(&(p->m));
}

void consuma(PriorityProdCons * p){
    int value=0;

    enter_monitor(&(p->m));

    while(p->riemp==0){
        wait_condition(&(p->m),VAR_COND_CONS);
    }

    value=p->buffer[p->testa];
    p->testa=(p->testa+1)%DIM;
    p->riemp--;

    signal_condition(&(p->m),VAR_COND_PRODH);
    signal_condition(&(p->m),VAR_COND_PRODL);

    leave_monitor(&(p->m));

    printf("Valore consumato: %d\n",value);
}

void rimuovi_prod_cons(PriorityProdCons * p){
    remove_monitor(&(p->m));
}