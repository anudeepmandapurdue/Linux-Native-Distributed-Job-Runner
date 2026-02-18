#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void run_worker(int id, int seconds) {
    printf("Worker %d: Starting (Will sleep for %d seconds)...\n", id, seconds);
    sleep(seconds); // Simulate a long job
    printf("Worker %d: Task Finished!\n", id);
    exit(0); // Terminate child
}

int main() {
    time_t start = time(NULL);

    // 1. Launch first worker
    if (fork() == 0) {
        run_worker(1, 2);
    }

    // 2. Launch second worker (Parent continues immediately)
    if (fork() == 0) {
        run_worker(2, 2);
    }

    // 3. Parent waits for BOTH
    printf("Parent: Waiting for workers to finish...\n");
    wait(NULL); 
    wait(NULL);

    time_t end = time(NULL);
    printf("Total execution time: %ld seconds\n", end - start);
    
    if ((end - start) < 4) {
        printf("RESULT: Success! Jobs ran simultaneously.\n");
    }

    return 0;
}