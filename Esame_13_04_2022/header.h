#define NUM_UPDATES 10
#define NUM_READS 20
#define NUM_READERS 3

#define REQ_UPDATER 1
#define REQ_READER 2

typedef struct{
	//TODO: completare con la struttura del messaggio di richiesta
	long tipo;
	pid_t client_pid;
	int value;
} req; 

typedef struct{
	//TODO: completare con la struttura del messaggio di risposta
	long tipo;
	int value;
} res; 


