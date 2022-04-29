
#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>


// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: server (<account-name> <balance>)+\n" );
  exit( 1 );
}


char list[ALIST_MAX][ANAME_MAX + 1]; 

int count = 0; 

float balance[ALIST_MAX]; 
// Flag for telling the server to stop running because of a sigint.
// This is safer than trying to print in the signal handler.
static int running = 1;

void sig_handler(int signo){
  if( signo == SIGINT){
    printf("\n");
    for(int i = 0; i < count; i++){
      printf("\t %s %.2f\n",list[i], balance[i]);
    } 
    running = 0; 
  }

}

int main( int argc, char *argv[] ) {

  // Parse command-line arguments.
  if ( argc < 3 || argc > 21 || (argc - 1)%2 != 0)
    usage();
 
  for(int i = 1; i < argc; i+=2){
	if(sscanf(argv[i], "%s",list[count]) != 1 || sscanf(argv[i+1], "%f",&balance[count]) != 1) usage(); 
   	count++; 
  }
  // Remove both queues, in case, last time, this program terminated
  // abnormally with some queued messages still queued.
  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );

  // Prepare structure indicating maximum queue and message sizes.
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = MESSAGE_LIMIT;

  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open( SERVER_QUEUE, O_RDONLY | O_CREAT, 0600, &attr );
  mqd_t clientQueue = mq_open( CLIENT_QUEUE, O_WRONLY | O_CREAT, 0600, &attr );
  if ( serverQueue == -1 || clientQueue == -1 )
    fail( "Can't create the needed message queues" );

  //signal for ctr+C
  signal(SIGINT, sig_handler);

  //holds message being sent
  char buffer[ MESSAGE_LIMIT ];
  char commandBuffer[ MESSAGE_LIMIT ];
  char bufferValue[ MESSAGE_LIMIT ];
  
  // Repeatedly read and process client messages.
  while ( running ) {
    //clears buffers
    memset(buffer, 0 , MESSAGE_LIMIT);
    memset(commandBuffer, 0 , MESSAGE_LIMIT);
    memset(bufferValue, 0 , MESSAGE_LIMIT);

    //receives command from client
    if(mq_receive( serverQueue , commandBuffer, sizeof(commandBuffer) , NULL )==-1){
      fail("");
    }
    
    mq_receive( serverQueue , buffer, sizeof(buffer) , NULL );
    mq_receive( serverQueue , bufferValue, sizeof(bufferValue) , NULL );

    if(strncmp("query", commandBuffer, MESSAGE_LIMIT) == 0){
      int found = 0; 
      float balanceFound = 0.0; 
      char arr[MESSAGE_LIMIT]; 
      for(int i = 0; i < count; i++){
         if(strcmp(list[i], buffer) == 0){
           found = 1; 
           balanceFound = balance[i]; 
           sprintf(arr, "%.2f", balanceFound);
           //memcpy(arr,&balanceFound,sizeof(balanceFound));
         }
      }
      if(found==1){
      mq_send(clientQueue, arr, sizeof(arr), 0);
      }
      else mq_send(clientQueue, "error", sizeof("error"), 0);
    }

    if(strncmp("debit", commandBuffer, MESSAGE_LIMIT) == 0){
      int found = 0; 
      float balanceFound = 0.0; 
      float debit = atof(bufferValue); 
      for(int i = 0; i < count; i++){
         if(strcmp(list[i], buffer) == 0 && debit > 0){
           balanceFound = balance[i]; 
           if(balanceFound - debit >= 0){
             found = 1; 
             balance[i] -= debit; 
           }
           //sprintf(arr, "%.2f", balanceFound);
           //memcpy(arr,&balanceFound,sizeof(balanceFound));
         }
      }
      if(found==1){
        mq_send(clientQueue, "success", sizeof("success"), 0);
      }
      else mq_send(clientQueue, "error", sizeof("error"), 0);
    }

    if(strncmp("credit", commandBuffer, MESSAGE_LIMIT) == 0){
      int found = 0; 
      float credit = atof(bufferValue); 
      for(int i = 0; i < count; i++){
         if(strcmp(list[i], buffer) == 0 && credit > 0){
            found = 1; 
            balance[i] += credit; 
         }
      }
      if(found==1){
        mq_send(clientQueue, "success", sizeof("success"), 0);
      }
      else mq_send(clientQueue, "error", sizeof("error"), 0);
    }
  }

  // Close our two message queues (and delete them).
  mq_close( clientQueue );
  mq_close( serverQueue );

  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );

  return 0;
}
