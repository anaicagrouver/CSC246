
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

// Number of iterations for each thread
#define ITERATIONS 500

// Declare two anonymous semaphores, one to let each of the threads
// go next.

sem_t pingNext;
sem_t pongNext;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Start routines for the two threads.

void *pingFunc(){
  for ( int i = 0; i < ITERATIONS; i++ ) {
    sem_wait( &pingNext );
    sem_post( &pongNext );
  }
  return 0;
}

void *pongFunc(){
  for ( int i = 0; i < ITERATIONS; i++ ) {
    sem_wait( &pongNext );
    sem_post( &pingNext );
  }
  return 0;
}


int main( int argc, char *argv[] ) {

  // Create two semephaores, one to let ping go next and one to let
  // pong go next.

  if( sem_init(&pingNext, 0, 1) == -1)
    fail("sem_init");
  if( sem_init(&pongNext, 0, 1) == -1)
    fail("sem_init");

  // Create each of the two threads.

  pthread_t ping;
  pthread_t pong;
  if ( pthread_create( &ping, NULL, pingFunc, NULL ) != 0 ) 
    fail( "Can't create child thread\n" );
  if ( pthread_create( &pong, NULL, pongFunc, NULL ) != 0 ) 
    fail( "Can't create child thread\n" );

  // Wait for them both to finish.

  pthread_join( ping, NULL );
  pthread_join( pong, NULL );

  // Destroy the two semaphores.
  if( sem_destroy(&pingNext) != 0 )
    fail( "Can't destroy sem\n" );
  if( sem_destroy(&pongNext) != 0 )
    fail( "Can't destroy sem\n" );

  return 0;
}