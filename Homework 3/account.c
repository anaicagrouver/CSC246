#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include "common.h"
#include <limits.h>
struct AccountList {
   int count;
   int complete;
   char list[ALIST_MAX][ANAME_MAX + 1]; 
   float balance[ALIST_MAX]; 
};


bool debit(struct AccountList* aList, char const *name, float delta){
   sem_t *lock = sem_open( "/agrouve2-account-lock" , 0);
   if ( lock == SEM_FAILED )
      printf( "Can't open tag semaphore" );
  bool result = false; 
  #ifndef UNSAFE
    sem_wait( lock );
  #endif
  int found = 0; 
  for(int i = 0; i < aList->count; i++){
    if(strcmp(aList->list[i], name) == 0){
        if(aList->balance[i] - delta >= 0){
             found = 1; 
             aList->balance[i] -= delta; 
           //memcpy(arr,&balanceFound,sizeof(balanceFound));
           break;
        }
    }
  }  
    if(found==1){
      result = true;
    }
    sem_post(lock); 
    sem_close(lock);
     return result; 
}

bool credit ( struct AccountList* aList, char const *name, float delta){
    int found = 0; 
    sem_t *lock = sem_open( "/agrouve2-account-lock" , 0);
    if ( lock == SEM_FAILED )
        printf( "Can't open tag semaphore" );
    bool result = false; 
    #ifndef UNSAFE
      sem_wait( lock );
    #endif
    for(int i = 0; i < aList->count; i++){
    if(strcmp(aList->list[i], name) == 0){
      if(delta >= 0){
        //if(INT_MAX - aList->balance[i] - 1 > delta) return
        found = 1; 
        aList->balance[i] += delta; 
        break;
        }
       }
    }  
    if(found==1)
      result =  true; 
    sem_post(lock); 
    sem_close(lock);
    return result; 
}

int query(struct AccountList* aList, char const *name){
   sem_t *lock = sem_open( "/agrouve2-account-lock" , 0);
    if ( lock == SEM_FAILED )
        printf( "Can't open tag semaphore" );
    #ifndef UNSAFE
      sem_wait( lock );
    #endif
    int found = 0; 
    float balanceFound = 0.0; 
    char arr[1024]; 
    for(int i = 0; i < aList->count; i++){
       //printf("%s \n", aList->list[i]);
       if(strcmp(aList->list[i], name) == 0){
         found = 1; 
         balanceFound = aList->balance[i]; 
         sprintf(arr, "%.2f", balanceFound);
         printf("%s \n", arr);
         //memcpy(arr,&balanceFound,sizeof(balanceFound));
         break;
     }
    }
    if(found != 1){
      sem_post(lock); 
      sem_close(lock);
      printf("error\n");
      return -1; 
    } 
    sem_post(lock); 
    sem_close(lock);
    return balanceFound;
}

void test(struct AccountList *aList, char const* name, int n){
  int remaining = 0;
  for(int i = 0; i < aList->count; i++){
     //printf("%s \n", aList->list[i]);
     if(strcmp(aList->list[i], name) == 0){
       //printf("remaining %d, n %d, balance %f\n",remaining, n, aList->balance[i]);
        if(INT_MAX - aList->balance[i] < n){
          //printf("hi"); 
          //printf("remaining %d, n %d, balance %f\n",remaining, n, aList->balance[i]);
          remaining = n - INT_MAX + aList->balance[i] ; 
          n -= remaining;
          //printf("remaining %d, n %d, balance %f\n",remaining, n, aList->balance[i]);
        }
        break;
     }
  }
  for(int i = 0; i < n; i++){
    credit(aList, name, 1);
  }
  for(int i = 0; i < n; i++){
    debit(aList, name, 1); 
  }
  //if(remaining != 0 )test(aList, name, remaining); 
}

int main( int argc, char *argv[] )
{
   int shmid;
   key_t key = ftok("/afs/unity.ncsu.edu/users/a/agrouve2/OS/starter-2-20220219", 1);
   struct AccountList *aList;
   shmid = shmget(key, sizeof(struct AccountList), 0644|IPC_CREAT);
     if (shmid == -1) {
      perror("Can't create shared memory");
      return 1;
   }
   aList = shmat(shmid, NULL, 0);
   if (aList == (void *) -1) {
      perror("Shared memory attach");
      return 1;
   }
   //while (aList->complete != 1); 
   if( argc == 3){
     if( strcmp(argv[1], "query") == 0)
     query(aList, argv[2]);
     else
     printf("error\n");
      // int found = 0; 
      // float balanceFound = 0.0; 
      // char arr[1024]; 
      // for(int i = 0; i < aList->count; i++){
      //    //printf("%s \n", aList->list[i]);
      //    if(strcmp(aList->list[i], argv[2]) == 0){
      //      found = 1; 
      //      balanceFound = aList->balance[i]; 
      //      sprintf(arr, "%.2f", balanceFound);
      //      printf("%s \n", arr);
      //      //memcpy(arr,&balanceFound,sizeof(balanceFound));
      //      break;
      //    }
      // }
      // if(found != 1) printf("error\n");
    //printf("%s\n", word);
  }

  else if( argc == 4){
    //checks for up command
    if( strcmp(argv[1],"debit") == 0){
      bool value = debit(aList, argv[2], atof(argv[3]));
      if(value == true)printf("success\n");
      else printf("error");
    }

    else if( strcmp(argv[1], "test") == 0){
       test(aList, argv[2], atoi(argv[3]));
     }

    else if( strcmp(argv[1],"credit") == 0){
       bool value = credit(aList, argv[2], atof(argv[3]));
       if(value == true)printf("success\n");
       else printf("error");
      }
      else{
        printf("error\n");
    }
  }
      else{
      printf("error\n");
    }

  // aList->complete = 2; 
  return 0;
}
