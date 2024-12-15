#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>

#define NUM_CLIENTS 3
#define NUM_WORKERS 3
#define NUM_COLLECTORS 1
#define NUM_MISURE 10

struct msg{
    long tipo;
    int id_client;
    int measure;
};

struct buffer{
    int somma;

    int num_update;

    pthread_mutex_t mutex;
    pthread_cond_t cv_prelievo;
};

struct param_worker{
    struct buffer* buf;
    int id;
};

void aggiorna(struct buffer *, int);
int preleva(struct buffer *);
