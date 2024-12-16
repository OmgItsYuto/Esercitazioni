#include "monitor_hoare.h"
#include <stdio.h>

#define DIM 5

typedef struct{
    int buffer[DIM];
    int testa;
    int coda;
    int nelem;

    Monitor m;

} BufferCircolare;

typedef struct{
    int buffer;
    int pieno; //bool

    Monitor m;
    
} BufferSingolo;

#define CV_CONS 0
#define CV_PROD 1

void produttore(BufferCircolare *, BufferSingolo *);
void consumatore(BufferCircolare *, BufferSingolo *);

