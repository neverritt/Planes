/* Name: Nathan Everritt
 * Filename: planes.c
 * Description: creates linked list of nodes, representing child processes
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int childId = 0;
int fuel = 100;

// interprets the user input - each char read, spaces divide the input
// and copy into args
void parse(char *input, char **args){
	while(*input != '\0') { // while not at end of input string
		while(*input == ' ' || *input == '\n')
			*input++ = '\0'; // change spaces to '\0' to separate args
		*args++ = input;  // save index of argument
		while(*input != '\0' && *input != ' ' && *input != '\n')
			input++;
	}
	*args = '\0';	
}

struct plane {
	int id;
	struct plane *next;
}*first;

void bomb(int signum){
	printf("Bomber %d to base, bombs away!\n", getpid());
	printf("Command: ");
}

void refuel(int signum){
	fuel = 100;
}

// searches for plane with the matching id
struct plane* searchList(struct plane *head, int id){
	while(head->id != id){
        	head=head->next;
  	}
    	struct plane* curr = head;
    	return curr;
}

void removeNode(struct plane *head, int id){
	struct plane *crash = searchList(head, id);    
	// node to be deleted is head plane
	if(head == crash){
        	if(head->next == NULL){
			//head->id = NULL;
			first = NULL;
			free(crash);
			return;
       		}
 
        	// copy id of next plane to head
		head->id = head->next->id;
 
       		// store address of next plane
		crash = head->next;
 
        	// Remove the link of next plane
        	head->next = head->next->next;
 
        	free(crash);
 
        	return;
	}
 
        // find the previous plane
	struct plane *prev = head;
	while(prev->next != NULL && prev->next != crash)
        	prev = prev->next;
 
        // check for plane in list
	if(prev->next == NULL){
        	printf("\n Given node is not present in Linked List");
        	return;
	}
 
        // remove plane from list
	prev->next = prev->next->next;
 
	free(crash);
 
	return; 
}

void removeSignal(int signum){
	childId = wait(NULL);
	removeNode(first, childId);
}

// add struct plane and begin new process
void launch(struct plane** head){
	struct plane *new = malloc(sizeof(struct plane*));
	
	int flag = 0;	
	int counter = 0;
	int countdown = 0;
	int id;
	id = fork();

	// if parent, add plane
	if (id != 0){
		new->id = id;
		new->next = *head;
		*head = new;
	} // if child, count down fuel
	else{
		while(1){
			if(fuel < 50){
				flag = 1;
				countdown = counter;
			} else {
				flag = 0;
				countdown = 0;
			}
			// if fuel runs out, free data, adjust list, and exit
			if(fuel == 0){
				printf("SOS! Plane has crashed\n");
				printf("Command: ");
				exit(0);
			}
			// sleep increments counter
			sleep(1);
			counter++;
			if((flag == 1 && countdown == counter) ||
			   (flag == 1 && countdown % 9 == 0)){
				printf("***Bomber %d to base, %d fuel left***\n", getpid(), fuel);
				printf("Command: ");
				fflush(stdout);
			}
			if(flag == 1)
				countdown++;
			if(counter % 3 == 0){
				fuel = fuel - 5;
			}
		}	
	}

}

// print id's of current planes
void status(struct plane* head){
	printf("The current planes are: ");
	while(head != NULL){
		printf("%d ", head->id);
		head = head->next;
	}
	printf("\n");
}

// end all plane processes
void end(struct plane* head){
	while(head != NULL){
		kill(head->id, SIGINT);
		head = head->next;
	}
}

int main(){

	setbuf(stdout,NULL);
	char input[128];
	char *args[64];
 
	// signal handling
	signal(SIGUSR1, bomb);
	signal(SIGUSR2, refuel);
	signal(SIGCHLD, removeSignal);
	
	int i;

	// first is the first plane pointer
	first = NULL;
	
	while(1){
		printf("Command: ");
		fflush(stdout);
		fgets(input,128,stdin);	
		
		// removing \n character
		if(input[strlen(input)-1] == '\n')
			input[strlen(input)-1] = '\0';	
		
		// parse input into args
		parse(input, args);

		// check input
		if(strcmp(args[0], "quit")==0 && args[1] == NULL){
			end(first); // ends all processes
			exit(0);
		} else
		
		if(strcmp(args[0], "launch")==0 && args[1] == NULL){
			launch(&first);
		} else

		if(strcmp(args[0], "status")==0 && args[1] == NULL){
			status(first);	
		} else

		if(strcmp(args[0], "bomb")==0 && args[1] != NULL && args[2] == NULL){
			// atoi converts args[1] into an int, to pass into kill
			i = atoi(args[1]);
			if(kill(i, SIGUSR1))
				printf("Invalid ID\n");		
		} else

		if(strcmp(args[0], "refuel")==0 && args[1] != NULL && args[2] == NULL){
			i = atoi(args[1]);
			if(kill(i, SIGUSR2))	
				printf("Invalid ID\n");
		} else

		{
			printf("That is not a valid command\n");
		}
	}

	return 0;
}
