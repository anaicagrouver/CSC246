#include <stdio.h>
#include <unistd.h>    // for write
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Define the start routines for your three threads.  Each thread will
// use two calls to write() to print its two characters, then it will
// terminate.

void *threadFunc( void *arg) {
  
  if(write(1, *(char *)arg ,1) != 1)
    fail( "Can't write arg 1" );
  if(write(1, *(char *)arg + 1 ,1) != 1)
    fail( "Can't write arg 2" );
  return NULL;
}

int main( int argc, char *argv[] ) {
  // A bunch of times.
  for ( int i = 0; i < 100000; i++ ) {
    // Make three threads.

    pthread_t thread[ 3 ];
    char threadArg[ 2 ];
    threadArg[0] = 'a';
    threadArg[1] = 'b';
    if ( pthread_create( &(thread[ 0 ]), NULL, threadFunc, threadArg) )
      fail( "Can't make child thread" );
    threadArg[0] = 'a';
    threadArg[1] = 'b';
    if ( pthread_create( &(thread[ 0 ]), NULL, threadFunc, threadArg) )
      fail( "Can't make child thread" );
    threadArg[0] = 'a';
    threadArg[1] = 'b';
    if ( pthread_create( &(thread[ 0 ]), NULL, threadFunc, threadArg) )
      fail( "Can't make child thread" );
    
    // Join with the three threads.
    
    pthread_join( thread[ 0 ], NULL );
    pthread_join( thread[ 1 ], NULL );
    pthread_join( thread[ 2 ], NULL );
    
    // Use the write system call to print out a newline.  The string
    // we're passing to write is null terminated (since that's what
    // double quotesd strings are in C), but we're just using the
    // first byte (the newline).  Write doesn't care about null
    // terminated strings, it just writes out any sequence of bytes
    // you ask it to.
    write( STDOUT_FILENO, "\n", 1 );
  }

  return 0;
}