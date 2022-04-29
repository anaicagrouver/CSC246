#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Number of iterations for each thread
#define ITERATIONS 500

// Who gets to go next.
int nextTurn = 0;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Start routines for the two threads.

void *ping( void *arg ) {
    for ( int i = 0; i < ITERATIONS; i++ ) {
     while ( nextTurn != 0 )
     ;
     nextTurn = 1;
    }
    return NULL;
}

void *pong( void *arg ) {
    for ( int i = 0; i < ITERATIONS; i++ ) {
     while ( nextTurn != 1 )
     ;
     nextTurn = 0;
    }
    return NULL;
}

int main( int argc, char *argv[] ) {

  // Create each of the two threads.

  pthread_t t_ping;
  pthread_t t_pong;

  if( pthread_create( &t_ping, NULL, ping, NULL ) != 0 )
    fail("Can't create threads");

  if( pthread_create( &t_pong, NULL, pong, NULL ) != 0 )
      fail("Can't create threads");

  // Wait for them both to finish.

  pthread_join(t_ping, NULL);
  pthread_join(t_pong, NULL);

  return 0;
}
