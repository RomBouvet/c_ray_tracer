/**
 * The ray tracor viewer.
 * @author Romain Bouvet, Kevin Fernagut
 * @version 10/04/2018
 **/
 
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>       
#include <sys/msg.h>  
#include <sys/shm.h>  
#include <sys/stat.h>   
#include <sys/socket.h>  /* Pour socket */
#include <arpa/inet.h>   /* Pour sockaddr_in, inet_pton */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */

#include "cst.h"
#include "scene.h"

    
int main(int argc, char* argv[]){
    int msgid,msg_buff,shmid,stop=0,udp_sock,tcp_sock,request,i,clients_nb=0;
    pid_t slaves_pid[CLIENT_NB];

    char buffer[256];

    struct sockaddr_in address,client;
    socklen_t addrlen;

    void *scene_pointer;
    scene_t *scene;

    if(argc!=6){
        fprintf(stderr,"Needs 6 parameters. See README\n");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for controller");
    /* Recuperation de la file */
    while((msgid = msgget((key_t)MSG_KEY, 0)) == -1) {
        sleep(1);
        printf(".");fflush(stdout);
    } 
    printf("\n");

    if(msgrcv(msgid, &msg_buff, sizeof(int), 0, 0) == -1) {
        perror("Erreur lors de la reception de la reponse ");
        exit(EXIT_FAILURE);    
    }

    printf("Got msg queue\n");

    if((shmid = shmget((key_t)SHM_KEY, 0, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    /* Attachement du segment de memoire partagee */
    if((scene_pointer = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    scene=scene_pointer;

    if(fork()==0){
        /* ncurses app */
        exit(EXIT_SUCCESS);
    }

    printf("Got shared memory\n");

    /* Creation de la socket */
    if((udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la creation de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Creation de l'adresse du serveur */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Nommage de la socket */
    if(bind(udp_sock, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1) {
        perror("Erreur lors du nommage de la socket ");
        exit(EXIT_FAILURE);
    }

    for(i=0;i<CLIENT_NB;i++){     
        if((slaves_pid[i]=fork())==0){
            /* Creation de la socket */
            if((tcp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
                perror("Erreur lors de la creation de la socket ");
                exit(EXIT_FAILURE);
            }

            address.sin_port = htons(atoi(argv[i+2]));
            if(bind(tcp_sock, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1) {
                perror("Erreur lors du nommage de la socket ");
                exit(EXIT_FAILURE);
            }
            if(listen(tcp_sock, 1) == -1) {
                perror("Erreur lors de la mise en mode passif ");
                exit(EXIT_FAILURE);
            }

            /* tcp part*/
        }else if(slaves_pid[i]<0){
            fprintf(stderr,"Error forking %d",i);
            exit(EXIT_FAILURE);
        }
    }

    addrlen=sizeof(client);

    while(!stop){
        if(recvfrom(udp_sock, &request, sizeof(int), 0, (struct sockaddr *)&client,(socklen_t*)&addrlen) == -1) {
            perror("Erreur lors de la reception de la taille du message ");
            exit(EXIT_FAILURE);
        }

        inet_ntop(AF_INET, &client.sin_addr, buffer, sizeof(buffer));
        printf("Client (%s) send : %d !\n",buffer,request);fflush(stdout);

        if(clients_nb<CLIENT_NB){
            request=202;
            ++clients_nb;
        }else
            request=507;

        if(sendto(udp_sock, &request, sizeof(int), 0, (struct sockaddr*)&client, sizeof(struct sockaddr_in)) == -1) {
            perror("Erreur lors de l'envoi de la taille du message ");
            exit(EXIT_FAILURE);
        }

        if(request==202){
            if(sendto(udp_sock, &i, sizeof(int), 0, (struct sockaddr*)&client, sizeof(struct sockaddr_in)) == -1) {
                perror("Erreur lors de l'envoi de la taille du message ");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* Detachement du segment de memoire partagee */
    if(shmdt(scene_pointer) == -1) {
        perror("Erreur lors du detachement ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}