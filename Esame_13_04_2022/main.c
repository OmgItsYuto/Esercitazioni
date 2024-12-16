#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int pid;
    key_t msg_req_key=ftok(".",'k');
	key_t msg_res_key=ftok(".",'j');

	int queue_req=msgget(msg_req_key,IPC_CREAT|0664);
	int queue_res=msgget(msg_res_key,IPC_CREAT|0664);

    pid = fork();
    if (pid==0) {
        //TODO exec per il server
        execl("./server","./server",NULL);
        _exit(0);
    }

    pid = fork();
    if (pid==0) {
        execl("./client","./client",NULL);
        _exit(0);
    }

    wait(NULL);
    wait(NULL);

    //TODO completare con rimozione delle code
    msgctl(queue_req,IPC_RMID,0);
    msgctl(queue_res,IPC_RMID,0);

    return 0;
}