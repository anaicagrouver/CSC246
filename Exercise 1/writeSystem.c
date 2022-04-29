#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 50000

int main( int argc, char *argv[] ) {
  // Buffer we're going to write to the file, one character at a time.
  char buffer[] = "abcdefghijklmnopqrstuvwxyz";
  int bufferSize = strlen( buffer );

  // Create an output file for writing.
  int  fd = open("output.txt", O_RDWR | O_CREAT, 0600);

  if (fd < 0) {
    fprintf( stderr, "Can't open output file.\n" );
    exit( EXIT_FAILURE );
  }

  for ( int i = 0; i < ITERATIONS; i++ ) {
    // Write out the buffer one character at a time.  That's not a
    // good idea, but it will help to show what system call overhead
    // is like.
    int count = 0 ;
    for ( int j = 0; j < bufferSize; j++ ){
        count +=1;
        write(fd, &buffer[j],1); 
        write(1, &buffer[j],1); 
    }
    lseek( fd , count ,SEEK_SET);
  }

  // Close the file and exit.
  close( fd );
  return EXIT_SUCCESS;
}

