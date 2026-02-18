#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int pipefd[2]; // different pipes (0) stdin (1) stdout (2) stderr.
    pid_t pid; //pid_t signed integer type used for storing process IDs, meant for storing IDs
    char buffer[1024]; //memory location for where the data is stored. allocates RAM to temporarily hold data pulled from the pipe


    //setup pipe (IPC Tunnel)
    //IPC tunnel 
    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(1);
    }
    // FORK: clone the parent process
    pid = fork();
    
    if(pid == 0){ //child process
        //conect the stdout to write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); //dup2 function: duplicate the file descriptor 2. takes the write end of the pipe and forces it to be stdout
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[0]);

        

        //EXEC: replace child node with 'ls'
        char *args[] = {"ls", "-la", NULL}; //commands and arguments for execution
        execvp(args[0], args); //reaplces child process code with ls program, "forgets" everything and starts running ls. 
        
    }
    else { //parent process
        close(pipefd[1]);

        // Loop to read until child closes its end of the pipe
        //pull data from kernel buffer into local RAM (buffer)
        int bytesRead;
        while((bytesRead = read(pipefd[0], buffer, sizeof(buffer)-1))> 0){ //read function asks kernel for data from the pipe. loop continues as the child is writing data. 
            buffer[bytesRead]  = '\0'; //index after the child is done writing the data.add null terminator for print f
            printf("Worker Output: %s", buffer);
        }   

        int status;
        waitpid(pid, &status, 0); //waits for child and gets result from pipefd[1]

        if(WIFEXITED(status)){
            int code = WEXITSTATUS(status);
            printf("Job finished with exit code: %d\n", code);
    
        }

    }
}
