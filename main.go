package main

import (
	"bufio"
	"fmt"
	"log"
	"os/exec"
)

func main() {
	// 1. Prepare the command (The C 'args' array)
	// We use 'ls -la' to test our ability to read process output.
	cmd := exec.Command("ls", "-la")

	// 2. Setup the pipe (The C 'pipe/dup2' logic)
	// This connects the child's stdout to our Go program.
	stdoutPipe, err := cmd.StdoutPipe()
	if err != nil {
		log.Fatal(err)
	}

	// 3. START the process (The C 'fork/exec' moment)
	// This triggers the OS to actually run the command.
	if err := cmd.Start(); err != nil {
		log.Fatal(err)
	}

	fmt.Println("Reading output in real time...")

	// 4. Read from the pipe as the child runs
	// Using a scanner allows us to stream logs line-by-line.
	scanner := bufio.NewScanner(stdoutPipe)
	for scanner.Scan() {
		fmt.Printf("Pipe Data: %s\n", scanner.Text())
	}

	// 5. Reap the process (The C 'waitpid')
	// This cleans up the zombie process and gets the exit code.
	if err := cmd.Wait(); err != nil {
		log.Fatal(err)
	}

	fmt.Println("Process finished and reaped.")
}