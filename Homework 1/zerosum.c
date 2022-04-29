/**
 * @file zerosom.c
 * @author Anaica
 * Program to find number of zerosum subarrays
 */

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <stdbool.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: zerosum <workers>\n" );
  printf( "       zerosum <workers> report\n" );
  exit( 1 );
}

// Input sequence of values.
int *vList;

// Number of values on the list.
int vCount = 0;

// Capacity of the list of values.
int vCap = 0;

// Read the list of values.
void readList() {
  // Set up initial list and capacity.
  vCap = 5;
  vList = (int *) malloc( vCap * sizeof( int ) );

  // Keep reading as many values as we can.
  int v;
  while ( scanf( "%d", &v ) == 1 ) {
    // Grow the list if needed.
    if ( vCount >= vCap ) {
      vCap *= 2;
      vList = (int *) realloc( vList, vCap * sizeof( int ) );
    }

    // Store the latest value in the next array slot.
    vList[ vCount++ ] = v;
  }
}

void getZeroSumSubarray(int child, int workers, bool report, int* pipe) {
    int count = 0; 
    for(int i = child; i < vCount; i+=workers){
      int sum = 0; 
      for(int j = i; j < vCount; j++){
        sum+= vList[j]; 
        if(sum == 0){
            count++;
            if(report)
              printf("%d .. %d\n", i,j); 
        }
      }
    }
    //lock and write to the pipe
    lockf( pipe[1], F_LOCK, 0);
    write( pipe[1], &count, sizeof( count ));

    //unlock the pipe
    lockf( pipe[1], F_ULOCK, 0);
    close( pipe[1]);
}  

int main( int argc, char *argv[] ) {
  bool report = false;
  int workers = 4;

  // Parse command-line arguments.
  if ( argc < 2 || argc > 3 )
    usage();
  
  if ( sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    usage();

  // If there's a second argument, it better be the word, report
  if ( argc == 3 ) {
    if ( strcmp( argv[ 2 ], "report" ) != 0 )
      usage();
    report = true;
  }

  readList();

  //create a pipe
  int pipes[2];
  if( pipe( pipes ) != 0 )
    fail("Unable to create pipe");

  //create workers
  for(int i = 0; i < workers; i++) {
      pid_t pid = fork();
      if( pid < 0 ) {
          fail("Unable to create children");
          return EXIT_FAILURE;
      }
      else if( pid == 0 ) {
          close(pipes[0]);
          getZeroSumSubarray(i, workers, report, pipes);
          return EXIT_SUCCESS;
      }
  }

  //close write
  close(pipes[1]);
  int total = 0;
  int temp = 0;
  for(int i = 0; i < workers; i++) {
      read(pipes[0], &temp, sizeof( temp ));
      total += (int) temp;
  }

  // wait for the child processes to end
  for(int i = 0; i < workers; i++)
    wait( NULL );

  printf("Total %d\n", total);
  return EXIT_SUCCESS;

  return 0;
}
