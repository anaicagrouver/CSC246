/**
  @program scrabbleServer.c
  @author Anaica Grouver
  Program that interacts with client using socket to play scrabble
*/
#include <stdio.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

/** Port number used by my server */
#define PORT_NUMBER "26048"

/** Maximum length of a user name. */
#define NAME_LEN 10

/** Number of alphabets. */
#define ALPHA_NUM 26

/** Maximum length of a players allowed. */
#define PLAYER_MAX 1000

/** Maximum length of a word. */
#define WORD_LEN 24

/**Semaphore for reporting **/
sem_t reporting;

/**Semaphore for submitting **/
sem_t submitting;

//scrabble score for all the letters, index -> alphabet
int SCORE[ALPHA_NUM] = {1,3,3,2,1,4,2,4,1,8,5,1,3,1,1,3,10,1,1,1,1,4,4,8,4,10}; 

//struct for maintaining player score and data
struct player {
   char*  username;
   char* latestWord;
   int   score;
};

//array of players
struct player players[PLAYER_MAX];  

//counter for number of players in game
int numOfPlayers = 0;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}
// https://www.geeksforgeeks.org/c-program-to-store-student-records-as-structures-and-sort-them-by-name/
int comparator(const void* p, const void* q){
    return (((struct player*)p)->score - ((struct player*)q)->score);
}

/** handle a client connection, close it when we're done. */

void *handleClient( void *socket_desc ) {
  // Here's a nice trick, wrap a C standard IO FILE around the
  // socket, so we can communicate the same way we would read/write
  // a file.
  int sock = *(int*)socket_desc;
  FILE *fp = fdopen( sock, "a+" );
  
  // Prompt the user for a name.
  fprintf( fp, "username> " );

  char user[ NAME_LEN + 2 ];
  if ( fscanf( fp, "%s", user ) != 1 ||
       strlen( user ) > NAME_LEN ) {
    fprintf( fp, "Invalid username\n" );
    fclose( fp );
    return NULL;
  }
  // Prompt the user for a command.
  fprintf( fp, "cmd> " );

  // Temporary values for parsing commands.
  char cmd[ 11 ];
  char word[ WORD_LEN + 2 ];
  while ( fscanf( fp, "%s", cmd ) == 1 &&
          strcmp( cmd, "quit" ) != 0 ) {
    // Just echo the command back to the client (for now)
    //strtok(cmd, " ")
    if(strcmp(cmd, "query") == 0){
      if(fscanf(fp, "%s", word) == 1);
      //fprintf( fp, "word %s\n", word );
      int result = 0; 
      if(strlen(word) > 24){
        fprintf(fp, "Invalid command\n"); 
      }
      else{
        for(int i = 0; i < strlen(word); i++){
          //calculating index for letter
          int idx = tolower(word[i]) - 'a';
          if(idx < 0 || idx > 25){
            fprintf(fp, "Invalid command\n"); 
            break;
          }
          //adding score of the word for each letter
          result += SCORE[idx]; 
        }
        fprintf(fp, "%d\n", result);
      }
    }

    else if(strcmp(cmd, "submit") == 0){
      if(fscanf(fp, "%s", word) == 1);
      int found = 0;
      //fprintf( fp, "submit word %s\n", word );
      int result = 0;
      if(strlen(word) > 24){
        fprintf(fp, "Invalid command\n"); 
      }    
      else{
        for(int i = 0; i < strlen(word); i++){
          //calculating index for letter
          int idx = tolower(word[i]) - 'a';
          if(idx < 0 || idx > 25){
            fprintf(fp, "Invalid command\n"); 
            break;
          }
          //adding score of the word for each letter
          result += SCORE[idx]; 
        }
        sem_wait(&submitting);
        for(int i = 0; i <numOfPlayers; i++){
          if(strcmp(players[i].username, user) == 0){
            //players[i].username = user;
            players[i].latestWord = word; 
            players[i].score = result;
            found = 1;
          }
        }
        if(!found){
          players[numOfPlayers].username = strdup(user);
          players[numOfPlayers].latestWord = word; 
          players[numOfPlayers++].score = result;
        }
        sem_post(&submitting);
      }  
    }
    else if(strcmp(cmd, "report") == 0){
      sem_wait(&reporting);
      qsort(players, numOfPlayers, sizeof(struct player), comparator);
      for(int i = 0; i <numOfPlayers; i++){
          fprintf(fp, "%-11s\t%-11s\t%-11d\n", players[i].username, players[i].latestWord,players[i].score);
      }
      sem_post(&reporting);
    } 

    else{
      fprintf(fp, "Invalid command\n"); 
    }

    // Prompt the user for the next command.
    fprintf( fp, "cmd> " );
    fflush( fp );
  }
  // Close the connection with this client.
  fclose( fp );
  return NULL;
}

int main() {

  //initializing semaphores
  sem_init(&reporting, 0, 1);
  sem_init(&submitting, 0, 1);

  // Prepare a description of server address criteria.
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_INET;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( NULL, PORT_NUMBER, &addrCriteria, &servAddr) )
    fail( "Can't get address info" );

  // Try to just use the first one.
  if ( servAddr == NULL )
    fail( "Can't get address" );

  // Create a TCP socket
  int servSock = socket( servAddr->ai_family, servAddr->ai_socktype,
                         servAddr->ai_protocol);
  if ( servSock < 0 )
    fail( "Can't create socket" );

  // Bind to the local address
  if ( bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) != 0 )
    fail( "Can't bind socket" );
  
  // Tell the socket to listen for incoming connections.
  if ( listen( servSock, 5 ) != 0 )
    fail( "Can't listen on socket" );

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  // Fields for accepting a client connection.
  struct sockaddr_storage clntAddr; // Client address
  socklen_t clntAddrLen = sizeof(clntAddr);
  //thread_id for handling sockets
  pthread_t thread_id;

  while ( true  ) {
    // Accept a client connection.
    int sock = accept( servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    //creating thread to handle multiple sockets at once 
    if( pthread_create( &thread_id , NULL ,  handleClient , (void*) &sock) < 0) {
            perror("could not create thread");
            return 1;
    }
    // //detaching thread 
    // int ret = pthread_detach(thread_id); 
    // Talk to this client.
    //handleClient( sock );
  }

  // Stop accepting client connections (never reached).
  close( servSock );
  return 0;
}
