
/**
 * @file exclude.c
 * @author Anaica
 * Program to copy content from one file to another while skipping the mentioned line
 */
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>

/**
* Converts string to int without using any c functions 
* @param a character array (string) of a number 
* @return the number converted to int 
*/
int stringToInt(char a[]){
    int n = 0; 
    for (int c = 0; a[c] != '\0'; c++) {
        n = n * 10 + a[c] - '0';
    }
    return n; 
} 


/**
* copies file content from first file to another while 
* ignoring the mentioned line number 
* @param argc number of arguments
* @param argv array of argument strings
* @return exit status of the program
*/
int main(int argc, char** argv){ 
    //error message
    char *buf = "usage: exclude <input-file> <output-file> <line-number>\n"; 

    if(argc != 4){
        write(1,buf, 56);
        _exit(1);
    }

    int n;
    char buffer[64];
    int  fd = open(argv[1], O_RDONLY);
    int  fd2 = open(argv[2], O_RDWR | O_CREAT, 0600);
    int line = stringToInt(argv[3]); 
    
    //throws error if either files are unable to open or less than 3 arguements are given
    if (fd < 0 || fd2 < 0 || argv[3] <= 0) {
        write(1,buf, 56);
        //https://www.tutorialspoint.com/unix_system_calls/exit.htm
        _exit(1);
    }

    int count = 1; 
    //reads 64 byte at once and writes byte by byte unless byte is in the line to be ignored
    while((n=read(fd, buffer, 64))>0){
        for(int i = 0; i < n; i++){
            //counting lines 
            if(buffer[i] == '\n'){
                count +=1; 
            }
            if(count == line || (line == 1 && count == 2 && buffer[i] == '\n' )){
                //ignore if char in line to be left out
                continue; 
            }
            else{
                //write to output file as long as char not in line to be ignored
                write(fd2, &buffer[i],1); 
            }
        }
    }
    //close the files
    close( fd2 );
    close( fd );
    return 1;
}

