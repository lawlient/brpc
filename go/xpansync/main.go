package main

import (
	"fmt"

	"xpansync/core"

	"github.com/gin-gonic/gin"
)

func main() {
	service, err := core.NewService()
	if err != nil {
		fmt.Println(err)
		return
	}

	if gin.Mode() == gin.ReleaseMode {
		fmt.Println("Release Mode")
		service.Engine().Use(embedFrontend())
	}

	service.Start()
}
