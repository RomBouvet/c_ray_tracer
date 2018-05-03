#define _XOPEN_SOURCE
 
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/shm.h>    

#include "scene.h"
#include "cst.h"
 
int main(int argc,char *argv[]) {
  int shmid;

  if(argc!=2){
      fprintf(stderr,"Wrong number of arguments\n");
      exit(EXIT_FAILURE);
  }
 
  /* Recuperation de la file */
  if((shmid = shmget((key_t)atoi(argv[1]),sizeof(scene_t),0)) == -1) {
    perror("Erreur lors de la recuperation du segment de memoire partagee ");
    exit(EXIT_FAILURE);
  }
 
  /* Suppression de la file */
  if(shmctl(shmid, IPC_RMID, 0) == -1) {
    perror("Erreur lors de la suppression de la file ");
    exit(EXIT_FAILURE);
  }
 
  printf("Shared memory cleared.\n");
 
  return EXIT_SUCCESS;
}