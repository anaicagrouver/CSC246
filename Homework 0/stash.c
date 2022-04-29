#define BUFFER 1024
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

/** holds the result of background child process*/
//char *result; 

/** 
*flag to check whether or not signals 
*from child process needs to be printed
*/
//int res; 

// /**
//  * Handle exit signals from child processes
//  * @param signal the signal from the child process
//  */
// void sig_handler(int signal) {
//   int status;
//   int result_ = wait(&status); 
//   res = 1;
//   result = (char *) malloc( BUFFER * sizeof( char ) );
//   sprintf(result, "[Done %d]\n", result_);
//   //printf("[Done %d]\n", result_);
// }

/**
 * parses the command by making pointer array 
 *point to each word of the command entered by user
 * @param line the line entered by user
 * @param words the array of pointers to point towards each word
 * @return count the number of words in line
 */
int parseCommand( char *line, char *words[] ){
    int pos = 0; 
    int posWords = 0;
    int count = 1;
    if(line[0]== '\0')count = 0;
    words[posWords++] = &line[pos++];
    while(line[pos] != '\0'){
        while(line[pos] != ' ' && line[pos] != '\t'){
            if(line[pos]=='\0') return count;
            pos++; 
        }
        if(line[pos] != '\0'){
            count+=1; 
            words[posWords++] = &line[pos+1];
            line[pos] = '\0'; 
        }
        //else return count; 
        pos++;
    }
    return count; 
}

/**
 * returns whether or not the string is an int
 * @param input the string to be checked
 * @return result of the check
 */
int isDigit(char* input){
    int length = strlen (input);
    for (int i=0; i < length; i++)
        if (!isdigit(input[i])){
            return -1;
        }
    return 1;
}

/**
 * Exits the program with given exit number
 * @param words the parsed command by user 
 * @param count the number of words in command 
 */
void runExit( char *words[], int count ){
    if(count == 1){
        exit(EXIT_SUCCESS);
    } 
    else if(count == 2){
        if(isDigit(words[1]) == 1){
            int value = atoi(words[1]);
            exit(value); 
        }
        else{
            printf("Invalid command\n");
        }
    }
    else{
        printf("Invalid command\n");
    }
    return; 
}

/**
 * Exits the program with given exit number
 * @param words the parsed command by user 
 * @param count the number of words in command 
 */
void runCd( char *words[], int count ){
    if(count > 2) {
        printf("Invalid command\n"); 
        return;
    }
    // if(words[1] == NULL) {
    //     char *home = getenv("HOME");
    //     if(chdir(home) != 0) {
    //         perror("Invalid command");
    //     }
    // }
    if(chdir(words[1]) != 0) {
            printf("Invalid command\n");
    }
    return;
}

void runCommand( char *words[], int count ){
    char* c = NULL; 
    words[count] = c; 
    int result = execvp(words[0], words) ;
    if(result == -1){
        printf("Can't run command %s\n", words[0]); 
    } 
    exit(EXIT_SUCCESS);
}

int ampersand(char *words[], int count){
    if(strcmp(words[count-1], "&") == 0){
        words[count-1] = NULL; 
        count -= 1; 
        return 1;
    }
    return 0; 
}

/**
 * Read the command line arguments for the shell
 * @param line char pointer to be populated after reading from stdin
 */
void readLine( char *line ) {
    int pos = 0;
    char ch;
    while( true ) {
        ch = getchar();
        if( ch == '\n' ) {
            line[pos] = '\0';
            return;
        }
        else{
            line[pos] = ch;
        }
        pos++;
        if( pos >= BUFFER ) {
            line = (char *) realloc(line, BUFFER * 2);
        }
    }
}

/**
 * Main function that handles program flow
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 * @return Exit status
 */

int main(int argc, char *argv[]){
    int x = 0; 
    int pid = -1;
    int background = 0; 
    while(x==0){
        printf("stash> "); 
        fflush(stdout);
        char *line; 
        line = (char *) malloc( BUFFER * sizeof( char ) );
        readLine( line );
        char **words; 
        words = malloc( (BUFFER/2 + 1) * sizeof( char *) );
        int count = parseCommand(line, words); 
        // if(res == 1){
        //     printf("%s", result);
        //     res = 0; 
        //     result = NULL; 
        // }
        if(count == 0){
            continue;
        }
        else{
            if(pid > 0 && background == 1){
                int status;
                waitpid(pid, &status, 0);
                if( WIFEXITED( status ) ) {
                    printf("[Done %d]\n", pid); 
                    pid = -1; 
                    background = 0;
                } 
            }
            background = ampersand(words, count); 
            //if(background == 1)sigset(SIGCHLD, sig_handler);
            if(strcmp(words[0], "cd") == 0){
                runCd(words,count);
            }
            else if(strcmp(words[0], "exit") == 0){
                runExit(words, count);
            }
            else if(count == 0){
                continue;
            }
            else{
                int status;
                pid = fork();
                if(pid == 0){
                    runCommand(words, count);
                    exit(1);
                }
                else{
                    if(background == 0){
                        waitpid(pid, &status, 0);
                        if( WIFEXITED( status ) != 0 ) {
                        WEXITSTATUS( status );
                        }
                    }
                    else{
                        printf("[%i]\n",pid);
                        //background = 0;
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS; 
}