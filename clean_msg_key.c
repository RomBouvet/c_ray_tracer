#define _XOPEN_SOURCE
 
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgctl */
 
int main(int argc,char *argv[]) {
  int msqid;

  if(argc!=2){
      fprintf(stderr,"Wrong number of arguments\n");
      exit(EXIT_FAILURE);
  }
 
  /* Recuperation de la file */
  if((msqid = msgget((key_t)atoi(argv[1]), 0)) == -1) {
    perror("Erreur lors de la recuperation de la file ");
    exit(EXIT_FAILURE);
  }
 
  /* Suppression de la file */
  if(msgctl(msqid, IPC_RMID, 0) == -1) {
    perror("Erreur lors de la suppression de la file ");
    exit(EXIT_FAILURE);
  }
 
  printf("File supprimee.\n");
 
  return EXIT_SUCCESS;
}