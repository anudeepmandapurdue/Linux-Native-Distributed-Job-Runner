
package main //one main package for the entire project --> tells complier that teh file should be an executable program
import (
	"bufio"
	"fmt" //similar ot print statement
	"log"
	"os/exec"
)

func main() {
	cmd := exec.Command("ls", "-la") //preparing command similar to char* args[]
	stdoutPipe, err := cmd.StdoutPipe() //manual pipe for stdout
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Reading output in real time") // manual reading, pulling bytes from pipe
	scanner := bufio.NewScanner(stdoutPipe)
	for scanner.Scan(){
		fmt.Printf("Pipe Data: %s\n", scanner.Text())
	}

	//reap the process
	// This is the equivalent of waitpid()
	if err := cmd.Wait(); err != nil {
		log.Fatal(err)
	}
	
	fmt.Println("Process finished and reaped.")
}