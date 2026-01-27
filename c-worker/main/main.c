#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int pipefd[2]; // different pipes (0) stdin (1) stdout (2) stderr
    pid_t pid; //pid_t signed integer type used for storing process IDs, meant for storing IDs
    char buffer[1024];


    //setup pipe (IPC Tunnel)
    //IPC tunnel 
    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(1);
    }
    // FORK: clone the parent process
    pid = fork();
    
    if(pid == 0){ //child process
        //conect the stdout to write end of the pipi
        dup2(pipefd[1], STDOUT_FILENO); //dup2 function: duplicate the file descriptor 2. takes the write end of the pipe and forces it to be stdout
        close(pipefd[0]);
        

        //EXEC: replace child node with 'ls'
        char *args[] = {"ls", "-la", NULL};
        execvp(args[0], args);

    }
    else { //parent process
        close(pipefd[1]);
    }
}