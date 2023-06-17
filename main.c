#include<stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SHC_RL_BUFSIZE 1024

#define SHC_TOK_BUFSIZE 64
#define SHC_TOK_DELIM " \t\r\n\a" 


char *lsh_read_line (){
    int bufsize = SHC_RL_BUFSIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "shellc: allocation error\n");
        exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
            }
        }
    }
}

 char** SHC_split_line(char *line){
    int bufsize = SHC_TOK_BUFSIZE, position = 0;
    char** tokens = malloc(bufsize * sizeof(char));
    char* token;

    if (!tokens) {
        fprintf(stderr, "SHC: allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHC_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += SHC_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "shc: allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SHC_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
 }

 int shc_launch (char** args) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        //child process
        if(execvp(args[0], args) == -1){
            perror("shc");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        //error forking
        perror("shc");
    } else {
        //parent process
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
 }