#include <stdio.h>
#include <pthread.h>

#define NUM_CLIENTS 3
#define NUM_WORKERS 2
#define DIM 3

struct msg_req{
    long tipo;
    pid_t client_pid;
    int values[2]; 
};

struct msg_res{
    long tipo;
    int product; 
};

struct buf_circolare{
    struct msg_req buf[DIM];
    int testa;
    int coda;
    int nelem;

    pthread_mutex_t mutex;
    pthread_cond_t cv_prod;
    pthread_cond_t cv_cons;
};

struct param_manager{
    struct buf_circolare *b;
    pthread_t th_w[NUM_WORKERS];
};