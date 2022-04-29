#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

static void fail( char *msg ) {
  fprintf( stderr, "Error: %s\n", msg );
  exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] ) {
  pid_t pid = fork();
  if ( pid == -1 )
    fail( "Can't create child process" );

  if ( pid == 0 ) {
    int input, output;
    input = open("input.txt", O_RDONLY);
    output = open("output.txt", O_WRONLY | O_CREAT, 0600);

    dup2(input, STDIN_FILENO);
    close(input);
    dup2(output, STDOUT_FILENO);
    close(output);
    if(execl("/bin/cat", "cat", NULL) == -1)
        fail("Execution failed");  
  }

  wait( NULL );

  return EXIT_SUCCESS;
}