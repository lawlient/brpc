package main

import (
	"fmt"

	"xpansync/core"
)

func main() {
	service, err := core.NewService()
	if err != nil {
		fmt.Println(err)
		return
	}

	service.Start()
}
