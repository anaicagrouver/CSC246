/**
 * Program to demonstrate thread synchronization using condition variables.
 * @author Anaica Grouver
 */
 
#include "diamond.h"
#include <stdio.h>
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit
#include <unistd.h>  

/** Mutex lock for the threads */
pthread_mutex_t lockOne = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockTwo = PTHREAD_MUTEX_INITIALIZER;

/** Condition variables for the cars to cross the diamond */
pthread_cond_t canTravelOne;
pthread_cond_t canTravelTwo;

/** State variables for cars travelling in various directions */
int nEast, nWest, sEast, sWest;
int waitN = 0; 
int waitS = 0;
/** Number of cars waiting to travel */
int carsOne = 0;
int carsTwo = 0;
int goAnywaySE = 0; 
int goAnywaySW = 0;
int goAnywayNE = 0; 
int goAnywayNW = 0;
/** Initialize the monitor. */
void initMonitor(){
    pthread_cond_init( &canTravelOne, NULL );
    pthread_cond_init( &canTravelTwo, NULL );
    nEast = 0;
    nWest = 0;
    sEast = 0;
    sWest = 0;
}

/** Destroy the monitor, freeing any resources it uses. */
void destroyMonitor(){
     pthread_cond_destroy( &canTravelOne );
     pthread_cond_destroy( &canTravelTwo );
}

/** Ehter the west side of the intersection, heading north-east. */
void enterNE( char const *name ){
    pthread_mutex_lock( &lockOne );
    carsOne++;
    nEast++;
    while( nWest  && goAnywayNE == 0) {
        pthread_cond_wait( &canTravelOne, &lockOne );
    }
    printf("Entering NE: %s\n", name );
    goAnywayNE = 0; 
    pthread_mutex_unlock( &lockOne );
}

/** Leave the west side of the intersection, heading north-east. */
void leaveNE( char const *name ){
    pthread_mutex_lock( &lockOne );
    // while(nWest){
    //     pthread_cond_wait( &canTravelOne, &lockOne );
    // }
    nEast--;
    carsOne--;
    printf("Leaving NE: %s\n", name );
    if(nWest && nEast) goAnywayNW = 1; 
    else if (nWest > 0) goAnywayNW = 1;
    else if(nEast > 0)goAnywayNE = 1;
    for(int i = 0; i <= carsOne; i++)
        pthread_cond_signal( &canTravelOne );
    pthread_mutex_unlock( &lockOne );
}

/** Ehter the west side of the intersection, heading north-west. */
void enterNW( char const *name ){
    pthread_mutex_lock( &lockOne );
    carsOne++;
    nWest++;
    while( nEast && goAnywayNW == 0 ) {
        pthread_cond_wait( &canTravelOne, &lockOne );
    }
    printf("Entering NW: %s\n", name );
    goAnywayNW = 0; 
    pthread_mutex_unlock( &lockOne );
}

/** Leave the west side of the intersection, heading north-west. */
void leaveNW( char const *name ){
    pthread_mutex_lock( &lockOne );
    // while(nEast){
    //     pthread_cond_wait( &canTravelOne, &lockOne );
    // }
    nWest--;
    carsOne--;
    printf("Leaving NW: %s\n", name );
    if(nWest && nEast) goAnywayNE = 1;
    else if (nWest > 0) goAnywayNW = 1;
    else if(nEast > 0)goAnywayNE = 1;
    for(int i = 0; i <= carsOne; i++)
        pthread_cond_signal( &canTravelOne );
    pthread_mutex_unlock( &lockOne );
}

/** Ehter the east side of the intersection, heading south-east. */
void enterSE( char const *name ){
    pthread_mutex_lock( &lockTwo );
    carsTwo++;
    sEast++;
    while( sWest  && goAnywaySE == 0) {
        pthread_cond_wait( &canTravelTwo, &lockTwo );
    }
        printf("Entering SE: %s\n", name );
    goAnywaySE = 0;
    pthread_mutex_unlock( &lockTwo );
}

/** Leave the east side of the intersection, heading south-east. */
void leaveSE( char const *name ){
    pthread_mutex_lock( &lockTwo);
    // while(sWest){
    //     pthread_cond_wait( &canTravelTwo, &lockTwo );
    // }
    sEast--;
    carsTwo--;
    printf("Leaving SE: %s\n", name );
    if(sWest > 0 && sEast  > 0)goAnywaySW = 1;
    else if (sWest > 0) goAnywaySW = 1;
    else if(sEast > 0)goAnywaySE = 1;
    for(int i = 0; i <= carsTwo; i++)
        pthread_cond_signal( &canTravelTwo );
    pthread_mutex_unlock( &lockTwo );
}

/** Ehter the east side of the intersection, heading south-west. */
void enterSW( char const *name ){
    pthread_mutex_lock( &lockTwo );
    carsTwo++;
        sWest++;
    while( sEast && goAnywaySW == 0) {
        pthread_cond_wait( &canTravelTwo, &lockTwo );
    }
    printf("Entering SW: %s\n", name );

    goAnywaySW = 0; 
    pthread_mutex_unlock( &lockTwo );
}

/** Leave the east side of the intersection, heading south-west. */
void leaveSW( char const *name ){
    pthread_mutex_lock( &lockTwo);
    // while(sEast){
    //     pthread_cond_wait( &canTravelTwo, &lockTwo );
    // }
    sWest--;
    carsTwo--;
    printf("Leaving SW: %s\n", name );
    if(sWest && sEast) goAnywaySE = 1; 
    else if (sWest > 0) goAnywaySW = 1;
    else if(sEast > 0)goAnywaySE = 1;


    for(int i = 0; i <= carsTwo; i++)
        pthread_cond_signal( &canTravelTwo );
    pthread_mutex_unlock( &lockTwo );
}