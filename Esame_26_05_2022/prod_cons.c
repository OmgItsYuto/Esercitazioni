#include "prod_cons.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void produttore(BufferCircolare *bc, BufferSingolo *bs){
    srand(time(NULL)*getpid());

    int num_produzioni=rand()%10;
    printf("[%d] - Produttore: numero produzioni -> %d\n",getpid(),num_produzioni);

    enter_monitor(&(bs->m));

    bs->buffer=num_produzioni;
    bs->pieno=1;

    signal_condition(&(bs->m),CV_CONS);

    leave_monitor(&(bs->m));

    for(int i=0;i<num_produzioni;i++){
        enter_monitor(&(bc->m));

        if(bc->nelem==DIM){
            wait_condition(&(bc->m),CV_PROD);
        }
        
        int value=rand()%10;
        bc->buffer[bc->testa]=value;
        bc->testa=(bc->testa+1)%DIM;
        bc->nelem++;
        printf("[%d] - Produttore: valore prodotto -> %d\n",getpid(),value);

        signal_condition(&(bc->m),CV_CONS);

        leave_monitor(&(bc->m));
    }
}

void consumatore(BufferCircolare *bc, BufferSingolo *bs){
    enter_monitor(&(bs->m));

    if(!bs->pieno)
        wait_condition(&(bs->m),CV_CONS);

    int num_consumazioni=bs->buffer;

    leave_monitor(&(bs->m));

    printf("[%d] - Consumatore: numero consumazioni da effettuare -> %d\n",getpid(),num_consumazioni);

    for(int i=0;i<num_consumazioni;i++){
        enter_monitor(&(bc->m));

        if(bc->nelem==0){
            wait_condition(&(bc->m),CV_PROD);
        }

        int value=bc->buffer[bc->coda];
        bc->coda=(bc->coda+1)%DIM;
        bc->nelem--;

        signal_condition(&(bc->m),CV_PROD);

        leave_monitor(&(bc->m));

        printf("[%d] - Consumatore: valore consumato -> %d\n",getpid(),value);
    }
}
