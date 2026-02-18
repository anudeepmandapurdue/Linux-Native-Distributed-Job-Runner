#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t global_child_pid = -1;

//function when the timer goes off
void handle_timeout(int sig){
    if(global_child_pid > 0){
        printf("\n[WATCHDOG]: Job timed out! Killing process %d...\n", global_child_pid);
        kill(global_child_pid, SIGKILL); // Send the "Death" signal
    }
}
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
        global_child_pid = pid; 
        printf("--- Watchdog Active (5s timeout) ---\n");
        alarm(5);
        FILE *log_file  = fopen("job_history.log", "a");
        if(log_file == NULL){
            perror("Failed to open log file");
        }
        // Loop to read until child closes its end of the pipe
        //pull data from kernel buffer into local RAM (buffer)
        int bytesRead;
        if (log_file){
            printf("--- Job Started: %s ---\n", argv[1]);
        }
        while((bytesRead = read(pipefd[0], buffer, sizeof(buffer)-1))> 0){ //read function asks kernel for data from the pipe. loop continues as the child is writing data. 
            buffer[bytesRead]  = '\0'; //index after the child is done writing the data.add null terminator for print f
            printf("%s", buffer);
            if (log_file){
                fprintf(log_file, "%s", buffer);

            }
        }   

        int status;
        waitpid(pid, &status, 0); //waits for child and gets result from pipefd[1]
        alarm(0);
        printf("\n--- Job Finished ---\n");
        
        if (WIFEXITED(status)) {
            printf("Status: Process exited normally with code %d\n", WEXITSTATUS(status));
        } 
        else if (WIFSIGNALED(status)) {
            // WTERMSIG tells us WHICH signal killed the process.
            // If it's 9, that's SIGKILL (our Watchdog did it).
            printf("Status: Process was KILLED by signal %d\n", WTERMSIG(status));
            if (WTERMSIG(status) == SIGKILL) {
                printf("Note: This was likely a Watchdog Timeout.\n");
            }
        }

        if (log_file) {
            fprintf(log_file, "FINAL STATUS: %d\n", status);
            fclose(log_file);
        }
        close(pipefd[0]);

    }
    return 0;
}
