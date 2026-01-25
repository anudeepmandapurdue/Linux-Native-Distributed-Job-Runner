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


}