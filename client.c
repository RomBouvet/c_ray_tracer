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
#include "cst.h"
    

int main(int argc, char* argv[]){
	int sockfd,tcpsockfd;
	dimensions_t dim;
	char msg='r', buffer;
 	struct sockaddr_in viewerAddress;
 
 	/*** args verification ***/
	if(argc!=4){
	 	fprintf(stderr,"Use : %s viewerAddress viewerUDPPort viewerTCPPort\n",argv[0]);
	 	fprintf(stderr,"where :\n-> viewerAddress is the @IP of viewer app.\n-> viewerUDPPort : n° of viewer's UDP Port.\n-> viewerTCPPort : n° of viewer's TCP Port.\n");  
	 	exit(EXIT_FAILURE);
	}
	
	/*** UDP socket's creation ***/
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Error while creating udp socket ");
		exit(EXIT_FAILURE);
	}
	/*** TCP socket's creation ***/
	if((tcpsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("Error while creating tcp socket ");
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
	if(sendto(sockfd,&msg,sizeof(unsigned char), 0, (struct sockaddr*)&viewerAddress, sizeof(struct sockaddr_in)) == -1) {
		perror("Error while sending request ");
		exit(EXIT_FAILURE);
	} 
	printf("Customer : request sent\n");
	
	/*** Waiting for viewer's answer ***/
	if(recvfrom(sockfd, &msg, sizeof(char), 0,NULL,NULL) == -1) {
		perror("Error while receiving message ");
		exit(EXIT_FAILURE);
	}
	/*** Check if connection is possible or not and verification of dimensions ***/
	if(msg=='k'){ /*** Impossible ***/
		fprintf(stderr,"Too much customers connected to the viewer. Try again later\n");
	} else { 	/*** Possible ***/
		printf("Waiting for dimensions...\n");
		if(recvfrom(sockfd, &dim, sizeof(dimensions_t),0,NULL,NULL) == -1) {
			perror("Error while receiving message ");
			exit(EXIT_FAILURE);
		}
		printf("Dims to calculate : %d x %d ",dim.height,dim.width);
		if(1/*DIMENSIONS POSSIBLES A CALCULER ??? */){
			printf("possible, etablishing TCP session.\n");
			msg='o';
		} else {
			printf("impossible, too heavy.\n");
			msg='k';
		}
		if(sendto(sockfd,&msg,sizeof(unsigned char),0,(struct sockaddr*)&viewerAddress,sizeof(struct sockaddr_in)) == -1) {
			perror("Error while sending answer ");
			exit(EXIT_FAILURE);
		} 
	}	
	/*** principal loop, with TCP connection ***/
	if(msg=='o'){
		viewerAddress.sin_port = htons(atoi(argv[3]));
		if(connect(tcpsockfd, (struct sockaddr*)&viewerAddress, sizeof(viewerAddress)) == -1) {
    			perror("Erreur lors de la connexion ");
    			exit(EXIT_FAILURE);
  		}
  		printf("Connexion TCP établie.\n");
  		while(1){
			printf("Client: \t");
			scanf("%c", &buffer);
			send(tcpsockfd, &buffer, 1, 0);
		}
  }
	
	/*** Socket closing ***/
	if(close(sockfd) == -1) {
		perror("Error while closing socket ");
		exit(EXIT_FAILURE);
	}
 	return EXIT_SUCCESS;
}

