#define SHC_RL_BUFSIZE 1024
#include<stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char *lsh_read_line (){
    int bufsize = SHC_RL_BUFSIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "shellc: allocation error\n");
        exit(1);
    }

    while(1){
        c = getchar;

        if(c == EOF || c == '\n') {
            buffer[position] = '\0';

            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += SHC_RL_BUFSIZE;
            buffer = realloc(buffer,bufsize);
            if(!buffer){
                fprintf(stderr, "shc: allocation error\n");
                exit(1);
            }
        }
        
    }

}