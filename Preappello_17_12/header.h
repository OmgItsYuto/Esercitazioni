// questo file è completo, nulla da implementare
#define MAX_CLIENTI 5
#define TURNI 5

typedef struct {
    long id_cliente;
    int valore_offerto;
} offerta_msg;

typedef struct {
    long id_cliente;
    int max_valore_offerto;
    int asta_aggiudicata;
    int id_vincitore;
} risposta_offerta_msg;