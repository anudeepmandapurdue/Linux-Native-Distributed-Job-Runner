#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    //check if user gave a command to run
    if(argc < 2){
        printf("Usage: %s <commands> <args>\n", argv[0]);
        return 1;
    };
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
        close(pipefd[1]);

        

        //EXEC: replace child node with 'ls'
       
        execvp(argv[1], &argv[1]); //reaplces child process code with ls program, "forgets" everything and starts running ls. 
        perror("Command failed to start");
        exit(1);
    }
    else { //parent process
        close(pipefd[1]);
        

        // Loop to read until child closes its end of the pipe
        //pull data from kernel buffer into local RAM (buffer)
        int bytesRead;
        printf("--- Job Started: %s ---\n", argv[1]);
        while((bytesRead = read(pipefd[0], buffer, sizeof(buffer)-1))> 0){ //read function asks kernel for data from the pipe. loop continues as the child is writing data. 
            buffer[bytesRead]  = '\0'; //index after the child is done writing the data.add null terminator for print f
            printf("%s", buffer);
        }   

        int status;
        waitpid(pid, &status, 0); //waits for child and gets result from pipefd[1]
        printf("\n--- Job Finished (Exit Code: %d) ---\n", WEXITSTATUS(status));
        
        if(WIFEXITED(status)){
            int code = WEXITSTATUS(status);
            printf("Job finished with exit code: %d\n", code);
    
        }
        else if(WIFSIGNALED(status)){
            printf("Job killed by signal");
        };

    }
    return 0;
}
