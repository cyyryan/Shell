
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
struct bg_info
{
	char status;
	char * arg;
	pid_t pid;
	struct bg_info * next;	
};//to keep each job's information


int main ( void )
{
	struct bg_info * head = NULL;
	struct bg_info * current = NULL;
	char cwd[1024] ="shell>";
	for (;;)
	{
		//check if there are process terminated, 
		if(head != NULL){
			pid_t pt = waitpid(0, NULL, WNOHANG);
			//if the pid is greater than zero, that means the child process has been terminated
			while(pt>0){
				if (head->pid == pt){
					printf("%s has terminated\n", head->arg);
					head = head->next;
				}
				else {
					struct bg_info * tmp = NULL;
					struct bg_info * pre = NULL;
					tmp = head;
					while(tmp->pid != pt){
					pre = tmp;
					tmp = tmp->next;
					}
					printf("%s has terminated\n", tmp->arg);
					pre->next = tmp->next;
					
				}
				pt = waitpid(0, NULL, WNOHANG);
			}
			
		}
		char 	*cmd = readline (cwd);  	//read the command line
		printf ("Got: [%s]\n", cmd);
		pid_t p;
		char** args=(char**)malloc(sizeof(char*)*15); //allocate the memory to a string array
		args[0]= strtok(cmd, " ");					//tokenize the command line and store in a string array
		int i = 0;
		while(args[i]!=NULL){
			args[i+1]= strtok(NULL, " ");
			i++;
		}
		if(args[0]==NULL){
			continue;
		}
		char* s=(char*)malloc(1024);				//a string to store the jobs

		int j = 1;
		while(args[j]!=NULL){
			strcat(s, args[j]);
			strcat(s, " ");
			j++;
		}
		//if the command is "cd"
		if (strcmp(args[0], "cd")==0){
			if(args[1]==NULL||strcmp(args[1],"~")==0)
			{
				chdir(getenv("HOME"));
			}
			else{
				chdir(args[1]);
				
			}
			getcwd(cwd,1024);
			strcat(cwd, ">");
		}
			else if (strcmp(args[0], "bg")==0)
			{
                args++;
                p = fork();
				//create a child process to run the job
                if(p == 0){
                    int rc;
                    rc = execvp(args[0], args);
                }
			else {
				//store the information of processes in the list
				if(head == NULL) {
					struct bg_info * tmp=(struct bg_info*) malloc(sizeof(struct bg_info));;
					tmp->status = 'R';
					tmp->arg = s;
					tmp->pid = p;
					tmp->next = NULL;
					head = tmp;
					current = tmp;
				}
				//append the list
				else {
					struct bg_info * tmp=(struct bg_info*) malloc(sizeof(struct bg_info));;
					tmp->status = 'R';
					tmp->arg = s;
					tmp->pid = p;
					tmp->next = NULL;
					current->next = tmp;
					current = tmp;
				}				
			}
			
		}
		//the part of background list, print all the information of the list
		else if (strcmp(args[0], "bglist")==0)
		{
			int i = 0;
			current = head;
			while(current != NULL){
				printf("%d[%c]: %s\n", i, current->status, current->arg);
				current = current->next;
				i++;
			}
			printf("Total Background jobs: %d\n", i);
		}
		//the part of killing background jobs
		else if (strcmp(args[0], "bgkill")==0)
		{
			int x =atoi(args[1]);
			struct bg_info * pre = NULL;
			int i = 0;
			current = head;
			while(current != NULL){
				
				if (x == i) {
					kill(current->pid, SIGKILL);
					break;
				}
				current = current->next;
				i++;
			}
		}
		//the part of restarting jobs
		else if (strcmp(args[0], "start")==0)
		{
			int x =atoi(args[1]);
			struct bg_info * pre = NULL;
			int i = 0;
			current = head;
			while(current != NULL){
				
				if (x == i) {
					if(current->status == 'S') 			//check if the job is currently runnung
					{
						kill(current->pid, SIGCONT);
						break;
					}
					else 
					{
						printf("the job is currently running\n");
						break;
					}
				i++;
				}
			}
			current->status = 'R';
		}
	
		else if (strcmp(args[0], "stop")==0)
		{
			int x =atoi(args[1]);
			struct bg_info * pre = NULL;
			int i = 0;
			current = head;
			while(current != NULL){
				
				if (x == i) {
					if(current->status == 'R') 				////check if the job is currently stopped
					{
						kill(current->pid, SIGSTOP);
						break;
						
					}
					else{
						printf("the job is currently stopped\n");
						break;
					}
					
					i++;
				}
			}
			current->status = 'S';
			
		}
		else {	
			p = fork();		
			if ( p == 0) {
				int rc;
				rc = execvp(args[0], args);	
			}		
			else {
				waitpid(p, NULL, 0);
			}
		}
		free(cmd);
	}
}	
	

