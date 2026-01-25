package main

import (
	"bytes"
	"os/exec"
	"strings"
	"testing"
)

func TestJobOutput(t *testing.T) {
	// 1. Define a command that prints a specific string
	cmd := exec.Command("echo", "hello-os")

	// 2. Capture the output into a buffer (Parent memory)
	var out bytes.Buffer
	cmd.Stdout = &out

	// 3. Execute the Fork/Exec/Wait sequence
	err := cmd.Run()
	if err != nil {
		t.Fatalf("OS failed to run command: %v", err)
	}

	// 4. Verify the data moved through the Pipe correctly
	got := strings.TrimSpace(out.String())
	want := "hello-os"

	if got != want {
		t.Errorf("IPC Error: got %q, want %q", got, want)
	}
}