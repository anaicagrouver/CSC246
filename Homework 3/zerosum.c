#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( EXIT_FAILURE );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: zerosum <workers>\n" );
  printf( "       zerosum <workers> report\n" );
  exit( 1 );
}

// True if we're supposed to report what we find.
bool report = false;

// Total number of subsequences found.
int total = 0;

// Fixed-sized array for holding the sequence.
#define MAX_VALUES 500000
int vList[ MAX_VALUES ];
bool visited[ MAX_VALUES ];
// Current number of values on the list.
int vCount = 0;

// all semaphores
sem_t semTotal;
sem_t semBool;
sem_t semWait;

//current final position of threads
int pos = 0;

bool isReading;

int getWork(){
  while(true){
   if(!isReading && pos >= vCount - 1){
     return -1;
   }else if(pos >= vCount - 1){
     sem_wait(&semWait); 
     continue; 
   }else{
     pos = vCount - 1; 
     return vCount - 1; 
   }
  }
}

// Read the list of values.
void readList() {
  // Keep reading as many values as we can.
  int v;
  isReading = true;
  while ( scanf( "%d", &v ) == 1 ) {
    // Make sure we have enough room, then store the latest input.
    if ( vCount > MAX_VALUES )
      fail( "Too many input values" );
    sem_wait(&semBool);  
    visited[vCount] = false; 
    sem_post(&semBool);
    vList[ vCount++ ] = v;
    sem_post(&semWait);
  }
  isReading = false;
}

/** Start routine for each worker. */
void *workerRoutine( void *arg ) {
  int position; 
  while((position = getWork()) != -1){
    for(int i = position; i >= 0 && !visited[i]; i--){
      sem_wait(&semBool);
      visited[position] = true; 
      sem_post(&semBool);
      int sum = 0; 
      for(int j = i; j >= 0; j--){
        sum += vList[j] ;
        if(sum == 0){
            sem_wait(&semTotal);
            total++;
            sem_post(&semTotal);
            if(report)
              printf("%d .. %d\n", i,j); 
        }
      }
    }
  }
  return NULL;
}

int main( int argc, char *argv[] ) {
  int workers = 4;
  
  // Parse command-line arguments.
  if ( argc < 2 || argc > 3 )
    usage();
  
  if ( sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    usage();

  // If there's a second argument, it better be "report"
  if ( argc == 3 ) {
    if ( strcmp( argv[ 2 ], "report" ) != 0 )
      usage();
    report = true;
  }

  // Make each of the workers.
  pthread_t worker[ workers ];

  //initialize semaphores
  sem_init(&semTotal, 0, 1);
  sem_init(&semBool, 0 ,1);
  sem_init(&semWait, 0, 1);


  for ( int i = 0; i < workers; i++ ) {
    pthread_create(&worker[i], NULL, workerRoutine, NULL);
  }

  // Then, start getting work for them to do.
  readList();

  // Wait until all the workers finish.
  for ( int i = 0; i < workers; i++ ) {
    pthread_join(worker[i], NULL);
  }

  // Report the total and release the semaphores.
  printf( "Total: %d\n", total );
  
  return EXIT_SUCCESS;
}
