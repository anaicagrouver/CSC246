/**
  @program client.c
  @author Anaica
  Client program that sends commands to server
*/

#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

int main( int argc, char *argv[] ) {
 
  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open( SERVER_QUEUE, O_WRONLY );
  mqd_t clientQueue = mq_open( CLIENT_QUEUE, O_RDONLY );
  if ( serverQueue == -1 || clientQueue == -1 )
    fail( "Can't create the needed message queues" );
  
  char word[MESSAGE_LIMIT];

  //checks for query command
  if( argc == 3 && strcmp(argv[1], "query") == 0){
    mq_send( serverQueue, argv[1] , strlen( argv[1] ), 0 );
    mq_send( serverQueue, argv[2] , strlen( argv[2] ), 0 );
    mq_send( serverQueue, argv[2] , strlen( argv[2] ), 0 );
    memset(word, 0, MESSAGE_LIMIT);
    mq_receive( clientQueue, word , MESSAGE_LIMIT , NULL);
    printf("%s\n", word);
  }
  
  if( argc == 4 ){
    //checks for up command
    if( strcmp(argv[1],"debit") == 0 ||  strcmp(argv[1],"credit") == 0){
      mq_send( serverQueue, argv[1] , strlen( argv[1] ), 0 );
      mq_send( serverQueue, argv[2] , strlen( argv[2] ), 0 );
      mq_send( serverQueue, argv[3] , strlen( argv[3] ), 0 );
      memset(word, 0, MESSAGE_LIMIT);
      mq_receive( clientQueue, word , MESSAGE_LIMIT , NULL);
      printf("%s\n", word);
    }  
  }
  //closes message queues
  mq_close( serverQueue );
  mq_close( clientQueue );
} 
