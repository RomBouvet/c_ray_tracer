/**
 * The ray tracor controller.
 * @author Romain Bouvet, Kevin Fernagut
 * @version 10/04/2018
 **/
 
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>  
#include <sys/msg.h>  
#include <unistd.h> 
#include <errno.h>      
#include <sys/stat.h>     
#include "util.h"

int main(int argc, char* argv[]){
 	int msqid,buf=6;
 	/*** message's queue creation ***/
	if((msqid=msgget((key_t)MSG_QUEUE_ID, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL))==-1){
		if(errno==EEXIST) {
			fprintf(stderr,"Queue ( key=%d ) already exist.\n",MSG_QUEUE_ID);
		} else {
			perror("Error while creating message's queue\n");
		}
		exit(EXIT_FAILURE);
	}
	
	sleep(10);
	if(msgsnd(msqid,&buf,sizeof(int),0)==-1){
		perror("Error while sending message ");
	} else {
		printf("Starting message sent to message's queue.\n");
	}	
	/*** Suppression de la file ***/
	if(msgctl(msqid, IPC_RMID, 0) == -1) {
		perror("Error while deleting message's queue ");
		exit(EXIT_FAILURE);
	}
 
	printf("Queue deleted.\n");
	return EXIT_SUCCESS;
}
