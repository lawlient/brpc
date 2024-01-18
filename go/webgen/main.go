package main

import (
	"errors"
	"fmt"
	"os"
	"time"
	"webgen/config"
	"webgen/generator"
)

func main() {
	start := time.Now()
	Prerequisite()

	var config config.Config
	config.Parse("config.json")

	var g generator.Generator
	g.Generate(&config)

	if _, err := os.Stat(config.Root() + "/ui"); errors.Is(err, os.ErrNotExist) {
		os.RemoveAll(config.Output)
		fmt.Println("/$ Generate failed, clear everything.")
		return
	}

	elapse := time.Since(start)
	fmt.Printf("/$ Generate successfully. Total time: %s\n", elapse)
}
