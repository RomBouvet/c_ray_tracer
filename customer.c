/**
 * A ray tracor customer.
 * @author Romain Bouvet, Kevin Fernagut
 * @version 10/04/2018
 **/
 
#include <stdlib.h>
#include <stdio.h>       
#include <sys/socket.h>  
#include <arpa/inet.h>   
#include <string.h>     
#include <unistd.h>  
#include <string.h>
#include "util.h"
    

int main(int argc, char* argv[]){
	int udpsockfd,tcpsockfd,possible=0;
	dimensions_t dim;
	char* msg;
 	struct sockaddr_in viewerAddress;
 	msg=(char*) malloc(sizeof(char));
 	*msg='r';
 
 	/*** args verification ***/
	if(argc!=4){
	 	fprintf(stderr,"Use : %s viewerAddress viewerUDPPort viewerTCPPort\n",argv[0]);
	 	fprintf(stderr,"where :\n-> viewerAddress is the @IP of viewer app.\n-> viewerUDPPort : n° of viewer's UDP Port.\n-> viewerTCPPort : n° of viewer's TCP Port.\n");  
	 	exit(EXIT_FAILURE);
	}
	
	/*** UDP socket's creation ***/
	if((udpsockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Error while creating socket ");
		exit(EXIT_FAILURE);
	}
		
	/*** viewer address creation ***/
	memset(&viewerAddress, 0, sizeof(struct sockaddr_in));
	viewerAddress.sin_family = AF_INET;
	viewerAddress.sin_port = htons(atoi(argv[2]));
	if(inet_pton(AF_INET, argv[1], &viewerAddress.sin_addr) == -1) {
		perror("Error during address conversion ");
		exit(EXIT_FAILURE);
	}
	
	/*** Sending request ***/
	if(sendto(udpsockfd,msg,sizeof(unsigned char), 0, (struct sockaddr*)&viewerAddress, sizeof(struct sockaddr_in)) == -1) {
		perror("Error while sending request ");
		exit(EXIT_FAILURE);
	} 
	printf("Customer : request sent\n");
	
	/*** Waiting for viewer's answer ***/
	if(recvfrom(udpsockfd, msg, sizeof(char), 0,NULL,NULL) == -1) {
		perror("Error while receiving message ");
		exit(EXIT_FAILURE);
	}
	/*** Check if connection is possible or not and verification of dimensions ***/
	if(strcmp(msg,"k")==0){ /*** Impossible ***/
		fprintf(stderr,"Too much customers connected to the viewer. Try again later\n");
	} else { 	/*** Possible ***/
		printf("Waiting for dimensions...\n");
		if(recvfrom(udpsockfd, &dim, sizeof(dimensions_t),0,NULL,NULL) == -1) {
			perror("Error while receiving message ");
			exit(EXIT_FAILURE);
		}
		printf("Dims to calculate : %d x %d ",dim.height,dim.width);
		if(1/*DIMENSIONS POSSIBLES A CALCULER ??? */){
			printf("possible, etablishing TCP session.\n");
			possible=1;
			*msg='o';
		} else {
			printf("impossible, too heavy.\n");
			*msg='k';
		}
		if(sendto(udpsockfd,msg,sizeof(unsigned char),0,(struct sockaddr*)&viewerAddress,sizeof(struct sockaddr_in)) == -1) {
			perror("Error while sending answer ");
			exit(EXIT_FAILURE);
		}
	}	
	/*** Main customer's loop ***/
	if(possible){
		sleep(1);
		/*** Etablishing TCP session ***/
		if((tcpsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			perror("Error while creating tcp socket ");
			exit(EXIT_FAILURE);
		}
		if(connect(tcpsockfd, (struct sockaddr*)&viewerAddress, sizeof(viewerAddress)) == -1) {
			perror("Error while connecting to viewer ");
			exit(EXIT_FAILURE);
		}
		printf("Boucle principale...");
	}
	
	/*** Socket closing ***/
	if(close(udpsockfd) == -1) {
		perror("Error while closing socket ");
		exit(EXIT_FAILURE);
	}
	/*** Memory deallocation ***/
	free(msg);
 	return EXIT_SUCCESS;
}

