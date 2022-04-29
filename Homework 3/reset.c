#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include "common.h"

// Print out an error message and exit.
static void fail( char const *message )
{
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message and exit.
static void usage()
{
  fprintf( stderr, "usage: reset (<account-name> <balance>)+\n" );
  exit( EXIT_FAILURE );
}

struct AccountList {
   int count;
   int complete;
   char list[ALIST_MAX][ANAME_MAX + 1]; 
   float balance[ALIST_MAX]; 
};


int main( int argc, char *argv[] )
{
   // Parse command-line arguments.
  if ( argc < 3 || argc > 21 || (argc - 1)%2 != 0)
    usage();
 
  struct AccountList *aList;
  sem_t *lock = sem_open("/agrouve2-account-lock", O_CREAT, 0600, 1 );
  if ( lock == SEM_FAILED )
    fail( "Can't create lock semaphore" );
  //sem_wait( lock );

  // Close and destroy the tag semaphore.
  sem_close( lock );
  //sem_unlink( "/agrouve2-account-lock" );
  key_t key = ftok("/afs/unity.ncsu.edu/users/a/agrouve2/OS/starter-2-20220219", 1);
  int shmid = shmget( key, sizeof(struct AccountList), 0644 | IPC_CREAT );
  if (shmid == -1) {
      perror("Can't create shared memory");
      return 1;
   }

  aList = shmat(shmid, NULL, 0);
  if ( aList == (void *)-1 )
    fail( "Can't map shared memory segment into address space" );
  //write into the buff
  aList->count = 0 ;
  for(int i = 1; i < argc; i+=2){
    aList->complete = 0;
    if(sscanf(argv[i], "%s",aList->list[aList->count]) != 1 || sscanf(argv[i+1], "%f",&(aList->balance[aList->count])) != 1) usage(); 
      aList->count++; 
    //bufptr = shmp->buf;
    //printf("%d heyyy anaica we work!\n",aList->count); 
  } 
  aList->complete = 1;

  //while ( aList->complete !=2 );

  if (shmdt(aList) == -1) {
      perror("aList");
      return 1;
   }

  //  if (shmctl(shmid, IPC_RMID, 0) == -1) {
  //     perror("shmctl");
  //     return 1;
  //  }
   printf("Writing Process: Complete\n");
  
  return 0;
}
