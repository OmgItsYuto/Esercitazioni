#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    printf("[%d] - Main iniziato\n",getpid());

    int pid;
    // completare l'inizializzazione code
    key_t msg_req_key = ftok(".",'m');
	key_t msg_res_key = ftok(".",'k');

	int queue_req = msgget(msg_req_key,IPC_CREAT|0664);
	int queue_res = msgget(msg_res_key,IPC_CREAT|0664);

    pid = fork();
    if (pid==0) {
        execl("./server","./server",NULL);
        _exit(0);
    }else if(pid<0){
        perror("Errore fork");
    }

    pid = fork();
    if (pid==0) {
        execl("./consumatore","./consumatore",NULL);
        _exit(0);
    }else if(pid<0){
        perror("Errore fork");
    }

    wait(NULL);
    wait(NULL);

    // eliminazione code
    msgctl(queue_req,IPC_RMID,0);
    msgctl(queue_res,IPC_RMID,0);

    return 0;
}
