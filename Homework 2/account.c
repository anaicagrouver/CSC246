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
#include "common.h" 

struct AccountList {
   int count;
   int complete;
   char list[ALIST_MAX][ANAME_MAX + 1]; 
   float balance[ALIST_MAX]; 
};

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
   if( argc == 3 && strcmp(argv[1], "query") == 0){
      int found = 0; 
      float balanceFound = 0.0; 
      char arr[1024]; 
      for(int i = 0; i < aList->count; i++){
         //printf("%s \n", aList->list[i]);
         if(strcmp(aList->list[i], argv[2]) == 0){
           found = 1; 
           balanceFound = aList->balance[i]; 
           sprintf(arr, "%.2f", balanceFound);
           printf("%s \n", arr);
           //memcpy(arr,&balanceFound,sizeof(balanceFound));
           break;
         }
      }
      if(found != 1) printf("error\n");
    //printf("%s\n", word);
  }

  if( argc == 4){
    //checks for up command
    if( strcmp(argv[1],"debit") == 0){
      int found = 0; 
      float debit = atof(argv[3]); 
      for(int i = 0; i < aList->count; i++){
         if(strcmp(aList->list[i], argv[2]) == 0){
           if(aList->balance[i] - debit >= 0){
             found = 1; 
             aList->balance[i] -= debit; 
           //memcpy(arr,&balanceFound,sizeof(balanceFound));
           break;
         }
      }
    }  
    if(found==1)
       printf("success\n");
    else printf("error\n");
    }
    else{
      //checks for up command
      if( strcmp(argv[1],"credit") == 0){
        int found = 0; 
        float debit = atof(argv[3]); 
        for(int i = 0; i < aList->count; i++){
          if(strcmp(aList->list[i], argv[2]) == 0){
            if(debit >= 0){
              found = 1; 
              aList->balance[i] += debit; 
            //memcpy(arr,&balanceFound,sizeof(balanceFound));
            break;
          }
        }
      }  
      if(found==1)
        printf("success\n");
      else printf("error\n");
      }
    }
   }

   aList->complete = 2; 
  return 0;
}
