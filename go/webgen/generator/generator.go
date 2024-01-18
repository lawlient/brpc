package generator

import (
	"errors"
	"fmt"
	"log"
	"os"
	"text/template"
	"webgen/config"
)

type Generator struct {
	config *config.Config

	vars      map[string]interface{}
	templates map[string]string

	server ServerGenerator
	ui     UIGenerator
}

func (g *Generator) Generate(config *config.Config) {
	g.config = config
	g.init()
	g.genRoot()
	g.genTemplates()

	g.server.generate()
	g.ui.generate()
}

func (g *Generator) init() {
	g.vars = make(map[string]interface{})
	g.vars["Project"] = g.config.Project
	g.vars["Port"] = g.config.Port
	g.vars["FrontendImage"] = g.config.FrontendImg
	g.vars["BackendImage"] = g.config.BackendImg
	g.vars["FrontendPort"] = g.config.FrontendPort
	g.vars["BackendPort"] = g.config.BackendPort

	g.templates = make(map[string]string)
	g.templates["readme.tmpl"] = "README.md"
	g.templates["dockerfile.tmpl"] = "Dockerfile"
	g.templates["docker-compose.dev.tmpl"] = "docker-compose.dev.yml"
	g.templates["docker-compose.tmpl"] = "docker-compose.yml"

	g.server.g = g
	g.ui.g = g
}

func (g *Generator) genRoot() {
	_, err := os.Stat(g.config.Root())
	if !errors.Is(err, os.ErrNotExist) {
		log.Fatal(g.config.Root(), " already exist")
	}

	err = os.MkdirAll(g.config.Root(), os.ModePerm)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("/# create %s successfully.", g.config.Root())
}

func (g *Generator) genTemplates() {
	for k, v := range g.templates {
		tmpl, err := template.ParseFiles(g.Template(k))
		if err != nil {
			fmt.Println(err)
			continue
		}

		file, err := os.Create(g.config.Root() + "/" + v)
		if err != nil {
			fmt.Println(err)
			continue
		}
		defer file.Close()

		tmpl.Execute(file, g.vars)
	}
}

func (g *Generator) Template(t string) string {
	return "generator/.template/" + t
}
