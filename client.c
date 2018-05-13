/**
 * The ray tracor client.
 * @author Romain Bouvet, Kevin Fernagut
 * @version 10/04/2018
 **/
 
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>       
#include <sys/socket.h>  /* Pour socket */
#include <arpa/inet.h>   /* Pour sockaddr_in, inet_pton */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */

#include "cst.h"
#include "scene.h"

int main(int argc, char *argv[]){
    int serv_sock,request,stop=0,part=-1;
    struct sockaddr_in address;

    picture_t picture;
    scene_t scene;

    if(argc!=4){
        fprintf(stderr,"Needs 6 parameters. See README\n");
        exit(EXIT_FAILURE);
    }

    /* Creation de la socket */
    if((serv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la creation de la socket ");
        exit(EXIT_FAILURE);
    }
    
    /* Creation de l'adresse du serveur */
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    if(inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) {
        fprintf(stderr,"Erreur lors de la conversion de l'adresse\n");
        exit(EXIT_FAILURE);
    }

    request=atoi(argv[3]);

    if(sendto(serv_sock, &request, sizeof(int), 0, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Erreur lors de l'envoi de la taille du message ");
        exit(EXIT_FAILURE);
    }

    if(recvfrom(serv_sock, &request, sizeof(int), 0, NULL,NULL) == -1) {
        perror("Erreur lors de la reception de la taille du message ");
        exit(EXIT_FAILURE);
    }

    switch(request){
        case 202:
            printf("Connected\n");
            if(recvfrom(serv_sock, &part, sizeof(int), 0, NULL,NULL) == -1) {
                perror("Erreur lors de la reception de la taille du message ");
                exit(EXIT_FAILURE);
            }
            break;
        case 507:
            printf("Max number of clients reached\n");
            exit(EXIT_FAILURE);
    }


    /* Fermeture de la socket */
    if(close(serv_sock) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    if((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Erreur lors de la creation de la socket ");
        exit(EXIT_FAILURE);
    }

    address.sin_port = htons(atoi(argv[3]));

    /* Connexion au serveur */
    if(connect(serv_sock, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Erreur lors de la connexion ");
        exit(EXIT_FAILURE);
    }

    picture_initialize(&picture,HEIGHT/2,WIDTH/2);

    while(!stop){
        if(read(serv_sock,&scene,sizeof(scene_t))==-1){
            perror("Erreur lors de la lecture de la taille du message ");
            exit(EXIT_FAILURE);
        }
    }

    picture_delete(&picture);

    return EXIT_SUCCESS;
}