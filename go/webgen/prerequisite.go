package main

import (
	"log"
	"os/exec"
)

// check prerequisite
func Prerequisite() {
	isInstalled("docker")
}

func isInstalled(name string) {
	path, err := exec.LookPath(name)
	if err != nil {
		log.Fatal(err)
	}

	log.Println(name, " installed:", path)
}
