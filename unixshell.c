#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
void commandExecution(char **);

int main() {
	char buffer[128];
	pid_t pid;
	int n;
	char* args[10];
	char* token;
	int bckgrnd;

	while(1){
		bckgrnd=0;
		printf("Shell> ");
		fgets(buffer, 127, stdin);
		if(buffer[0] == '\n')continue;
		buffer[strlen(buffer) - 1] = '\0';

		//tokenizzazione del comando
		token = strtok(buffer, " ");

		args[0] = token;

		n = 1;
		while(token != NULL && n<10) {

			token = strtok(NULL, " ");

			args[n] = token;

			n++;
		}

		args[9] = (char *)0;

		if(!strcmp(args[0], "exit")){
			return 0;
		}

		int i = 0;
		while(args[i] != NULL){
			if(*(args[i])=='&'){
				bckgrnd=1;
				args[i] = NULL;
			}
			i++;
		}

		pid = fork();

		if(pid == -1){
			//printf("Errore: fork non riuscita!\n");
			perror("Errore: fork non riuscita!");
			return 0;
		}
		else if(pid == 0){
			if (bckgrnd==1){
				pid_t childpid=fork();

				if(childpid == -1){
					//printf("Errore: fork non riuscita!\n");
					perror("Errore: fork non riuscita!");
					return 0;
				}else if(childpid==0){
					commandExecution(args);	
				}else{
					exit(0);
				}
			}else{
				commandExecution(args);
			}
		}else if(bckgrnd==0){
			pid = waitpid(pid,NULL,0);
		}
	}
	
	return 0;
}

void commandExecution(char *args[]){
	int i=0;
					
	execvp(args[0], args);
	//printf("Errore: exec non riuscita!\n");
	perror("Errore: exec non riuscita!");	
	exit(1);	
}