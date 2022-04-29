#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void fail( char *msg ) {
  printf( "Error: %s\n", msg );
  exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] ) {
  int pid, pid1, pid2;
  pid = fork();
  if ( pid == -1 )
    fail( "Can't create child process" );

  // Print out a report from that child.
  if ( pid == 0 ) {
    sleep( 1 );
    printf("I am %d, child of %d\n",
               getpid(), getppid());
    exit( EXIT_SUCCESS );
  }

 else {
        pid1 = fork();
        if (pid1 == 0) {
            sleep(1);
            printf("I am %d, child of %d\n",
               getpid(), getppid());
        }
        else {
            pid2 = fork();
            if (pid2 == 0) {
               sleep(1);
               printf("I am %d, child of %d\n",
               getpid(), getppid());
            }
            else {
                int nbChild = 3;
                while (nbChild) {
                    int stoppedChild = waitpid(WAIT_ANY, NULL, 0);
                    if(stoppedChild > 0)
                    --nbChild;
                }
                printf("Done\n");
            }
        }
    }

  return EXIT_SUCCESS;
}