/**
 * The ray tracor viewer.
 * @author Romain Bouvet, Kevin Fernagut
 * @version 10/04/2018
 **/
 
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>       
#include <sys/socket.h>  
#include <sys/msg.h>  
#include <arpa/inet.h>   
#include <string.h>     
#include <unistd.h> 
#include "cst.h"
    
int main(int argc, char* argv[]){
	/*** Declaration ***/
	int msg_id,msg_buff,i,sock_udp,customers_nb=0;
	int socks_udp[MAX_CUSTOMERS];

	char* msg;
 	struct sockaddr_in address;

 	socklen_t len;
 	dimensions_t dim[MAX_CUSTOMERS];
 	char buffer[512];
 	
 	/*** args verification ***/
	if(argc!=2+MAX_CUSTOMERS){
	 	fprintf(stderr,"Use : %s UDPPort TCPPort[1] ... TCPPort[n] \n",argv[0]);
	 	fprintf(stderr,"where :\n-> UDPPort : n° of viewer's UDP Port.\n-> TCPPort[n] : the differents n° of TCP Ports (1 for each possible customer -> here : %d)\n",MAX_CUSTOMERS);  
	 	exit(EXIT_FAILURE);
	}
	
 	
 	/*** Initialization ***/
 	len=sizeof(struct sockaddr_in);
 	msg=(char*) malloc(sizeof(char));
 	if((msqid = msgget((key_t)MSG_KEY, 0)) == -1) {
   	perror("Error while getting message's queue ");
   	exit(EXIT_FAILURE);
 	}
 	for(i=0;i<MAX_CUSTOMERS;i++){
 		dim[i].height=(MAX_HEIGHT/MAX_CUSTOMERS);
 		dim[i].width=(MAX_WIDTH/MAX_CUSTOMERS);
 	}
 	
	/*** Waiting for a message in msg's queue from controller ***/
	printf("Waiting for controller...\n");
	if(msgrcv(msqid,&buf,sizeof(int),0,0) == -1) {
		perror("Error while receiving message from queue ");
    	exit(EXIT_FAILURE); 
	} else {
		printf("C'est parti!\n");
	}
 	
 	/*** socket's creation ***/
	if((udpSockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Error while creating socket ");
		exit(EXIT_FAILURE);
	}
	for(i=0;i<MAX_CUSTOMERS;i++){
		if((sockfd[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			perror("Error while creating socket ");
			exit(EXIT_FAILURE);
		}
	}
	
	/*** viewer address creation ***/
	memset(&Address, 0, sizeof(struct sockaddr_in));
	Address.sin_family = AF_INET;
	Address.sin_port = htons(atoi(argv[1]));
	Address.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/*** udp socket bind ***/
	if(bind(udpSockFd, (struct sockaddr*)&Address, sizeof(struct sockaddr_in)) == -1) {
		perror("Error while binding udp socket ");
		exit(EXIT_FAILURE);
	}
	
	/*** tcp sockets bind ***/
	for(i=2;i<2+MAX_CUSTOMERS;i++){
		Address.sin_port = htons(atoi(argv[i]));
		if(bind(sockfd[i-2], (struct sockaddr*)&Address, sizeof(struct sockaddr_in)) == -1) {
			perror("Error while binding tcp socket ");
			exit(EXIT_FAILURE);
		}
		if(listen(sockfd[i-2], 1) == -1) {
			perror("Erreur lors de la mise en mode passif ");
			exit(EXIT_FAILURE);
		}
	}
	while(1){
		/*** Waiting for UDP request ***/
		if(recvfrom(udpSockFd, msg, sizeof(char), 0,(struct sockaddr*)&client, &len) == -1) {
			perror("Error while receiving message ");
			exit(EXIT_FAILURE);
		}
		printf("Query from customer (%s:%d)\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		
		/*** Verification if there's not too many customers ***/
		if(nbCustomers+1<=MAX_CUSTOMERS){
			*msg='o'; /* OK */
		} else {
			*msg='k'; /* KO */
		}
	
		/*** Answer to customer's UDP request (no or yes + dimensions) ***/
		if(sendto(udpSockFd,msg,sizeof(unsigned char),0,(struct sockaddr*)&client, sizeof(struct sockaddr_in)) == -1) {
			perror("Error while sending answer ");
			exit(EXIT_FAILURE);
		}
		
		if(strcmp(msg,"o")==0){ /*** If there isn't too many customers ***/
			if(sendto(udpSockFd,&dim[nbCustomers],sizeof(dimensions_t),0,(struct sockaddr*)&client, sizeof(struct sockaddr_in)) == -1) {
				perror("Error while sending dimensions ");
				exit(EXIT_FAILURE);
			} 
			printf("Viewer : answer and dimensions sent to %s:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
			/*** Waiting for customer's answer, to know if he can calculate these dimensions or not ***/
			if(recvfrom(udpSockFd, msg, sizeof(char),0,(struct sockaddr*)&client, &len) == -1) {
				perror("Error while receiving message ");
				exit(EXIT_FAILURE);
			}
			if(strcmp(msg,"o")==0){
				printf("The customer (%s:%d) can calculate these dimensions. Etablishing TCP session...\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
				if((tcpSockFd[nbCustomers] = accept(sockfd[nbCustomers],(struct sockaddr*)&Clients[nbCustomers], &len)) == -1) {
						perror("Erreur lors de la connexion ");
						exit(EXIT_FAILURE);
  					}
  					printf("Connexion TCP établie avec %s:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
  					nbCustomers++;
  					if(fork()==0){
  						close(sockfd[nbCustomers-1]);
  						while(1){
							recv(tcpSockFd[nbCustomers-1], buffer, 512, 0);
							if(strcmp(buffer,"exit")==0){
								printf("Deconnexion du client %d (%s:%d)\n",nbCustomers-1,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
								break;
							}
							printf("Client %d (%s:%d) sent : '%s'\n", nbCustomers-1,inet_ntoa(client.sin_addr),ntohs(client.sin_port), buffer);
							memset(buffer, 0, sizeof(buffer));
						}
  					}
			} else {
				printf("The customer (%s:%d) can't calculate these dimensions. Abort.\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
			}
		} else { /*** If there's too much customers ***/
			printf("Viewer : answer sent. this customer can't connect, too much.\n");
		}
		
	}
	
	/*** Socket closing ***/
	if(close(udpSockFd) == -1) {
		perror("Error while closing udp socket ");
		exit(EXIT_FAILURE);
	}
	for(i=0;i<MAX_CUSTOMERS;i++){
		if(close(tcpSockFd[i]) == -1) {
			perror("Error while closing tcp socket ");
			exit(EXIT_FAILURE);
		}
	}
	
	/*** Memory deallocation ***/
	free(msg);
		
	return EXIT_SUCCESS;
}
		
