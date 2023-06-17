#include<stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SHC_RL_BUFSIZE 1024

#define SHC_TOK_BUFSIZE 64
#define SHC_TOK_DELIM " \t\r\n\a" 

int main(int argc, char **argv){
    // load config files if any

    // run command loop
    shc_loop();

    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}

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

// function declarations of builtin shell commands:

 int shc_cd(char **args);
 int shc_help(char** args);
 int shc_exit(char **args);

//  list of builtin commands, followed by their corresponding functions 

 char *builtin_str[] = {
    "cd",
    "help",
    "exit"
 };

 int (*builtin_func[]) (char **) = {
    &shc_cd,
    &shc_help,
    &shc_exit
 };

 int shc_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
 }

//  Builtin function implementions

int shc_cd(char **args){
    if (args[1] == NULL) {
        fprintf(stderr, "shc: expceted argument to \"cd\"\n");
    } else{
        if (chdir(args[1]) != 0) {
            perror("shc");
        }
    }
    return 1;
}

int shc_help(char** args){
    int i;
    printf("ShellC\n");
    printf("type program names and arguments, and hit enter. \n");
    printf("the following are built-in:\n");

    for(i = 0; i < shc_num_builtins(); i++) {
        printf(" %s\n, builtin_str[i]");
    }

    printf("use the man command for information on other programs.\n");
}

int shc_exit(char** args){
    return 0;
}

int shc_execute(char** args){
    int i;

    if (args[0] == NULL) {
        //an empty command was entered
        return 1;
    }

    for (i = 0; i < shc_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);

        }
    }

    return shc_launch(args);
}