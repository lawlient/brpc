package generator

import (
	"fmt"
	"log"
	"os"
	"text/template"
	"webgen/docker"

	"github.com/docker/docker/api/types/container"
)

type ServerGenerator struct {
	g *Generator

	templates map[string]string

	sdk docker.DockerSDK
}

func (g *ServerGenerator) generate() error {
	g.templates = make(map[string]string)
	g.templates["Dockerfile.tmpl"] = "Dockerfile"
	g.templates["entrypoint.tmpl"] = "entrypoint.sh"
	g.templates["main.tmpl"] = "main.go"
	g.templates["cors.tmpl"] = "middleware/cors.go"

	err := os.MkdirAll(g.g.config.Root()+"/server/middleware", os.ModePerm)
	if err != nil {
		fmt.Println(err)
		return err
	}

	if err := g.startContainer(); err != nil {
		return err
	}
	g.generateTemplates()
	g.modTidy()
	g.sdk.Remove()
	return nil
}

func (g *ServerGenerator) startContainer() error {
	config := container.Config{
		// User:       util.User(),				 /* go mod tidy need root permission */
		Image:      g.g.config.BackendImg,
		Cmd:        []string{"/bin/sh"},
		WorkingDir: "/app/server",
		Env:        []string{"GOPROXY=https://goproxy.cn"}, // 加速go get
		Tty:        true,
	}

	hostconfig := container.HostConfig{
		Binds: []string{
			g.g.config.Root() + "/server:/app/server",
		},
	}

	if err := g.sdk.Run(&config, &hostconfig); err != nil {
		fmt.Println(err)
		return err
	}
	return nil
}

func (g *ServerGenerator) generateTemplates() {
	for k, v := range g.templates {
		tmpl, err := template.ParseFiles(g.Template(k))
		if err != nil {
			log.Fatal(err)
		}

		file, err := os.Create(g.g.config.Root() + "/server/" + v)
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()

		tmpl.Execute(file, g.g.vars)
	}
}

func (g *ServerGenerator) modTidy() {
	g.sdk.Exec2("go mod init " + g.g.config.Project)

	g.sdk.Exec2("go mod tidy")
}

func (g *ServerGenerator) Template(t string) string {
	return g.g.Template("/server/" + t)
}
