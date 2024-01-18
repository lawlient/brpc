package generator

import (
	"errors"
	"fmt"
	"os"
	"text/template"
	"webgen/docker"
	"webgen/util"

	"github.com/docker/docker/api/types/container"
)

type UIGenerator struct {
	g *Generator

	templates map[string]string

	sdk docker.DockerSDK
}

func (g *UIGenerator) generate() error {
	g.templates = make(map[string]string)
	g.templates["Dockerfile.tmpl"] = "Dockerfile"
	g.templates["entrypoint.tmpl"] = "entrypoint.sh"
	g.templates["env.development.tmpl"] = ".env.development"
	g.templates["env.production.tmpl"] = ".env.production"
	g.templates["main.tmpl"] = "src/main.jsx"
	g.templates["App.tmpl"] = "src/App.jsx"
	g.templates["Root.tmpl"] = "src/Root.jsx"
	g.templates["NotFound.tmpl"] = "src/pages/NotFound.jsx"
	if g.g.config.Tailwindcss {
		g.templates["index.tmpl"] = "src/index.css"
	}

	if err := g.startContainer(); err != nil {
		return err
	}
	g.createApp()
	g.sdk.Remove()
	g.generateTemplates()
	return nil
}

func (g *UIGenerator) startContainer() error {
	config := container.Config{
		User:       util.User(),
		Image:      g.g.config.FrontendImg,
		Cmd:        []string{"/bin/sh"},
		WorkingDir: "/app",
		Tty:        true,
	}

	hostconfig := container.HostConfig{
		Binds: []string{
			g.g.config.Root() + ":/app",
		},
	}

	err := g.sdk.Run(&config, &hostconfig)
	if err != nil {
		fmt.Println(err)
		return err
	}
	return nil
}

func (g *UIGenerator) createApp() {
	g.sdk.Exec2("npm config set registry https://registry.npm.taobao.org")

	// 此处耗时不定，取决于网络情况
	g.sdk.Exec2("npm create vite@latest ui -- --template react")

	if _, err := os.Stat(g.g.config.Root() + "/ui"); errors.Is(err, os.ErrNotExist) {
		// create 超时
		return
	}
	g.sdk.Exec2("cd ui && npm install")

	g.sdk.Exec2("cd ui && npm install --save react-router-dom")

	g.sdk.Exec2("cd ui && npm install --save axios")

	if g.g.config.Tailwindcss {
		g.sdk.Exec2("cd ui && npm install -D tailwindcss postcss autoprefixer")
		g.sdk.Exec2("cd ui && npx tailwindcss init -p")
		// todo: add content in tailwind.config.js
	}

	if g.g.config.UI == "joyui" {
		g.sdk.Exec2("cd ui && npm install @mui/joy @emotion/react @emotion/styled")
	} else if g.g.config.UI == "primereact" {
		g.sdk.Exec2("cd ui && npm install primereact")
	} else {
		if g.g.config.UI != "" {
			fmt.Println("/$ UNKNOWN UI: ", g.g.config.UI)
		}
	}

}

func (g *UIGenerator) generateTemplates() {
	_, err := os.Stat(g.g.config.Root() + "/ui")
	if errors.Is(err, os.ErrNotExist) {
		return
	}
	if err := os.MkdirAll(g.g.config.Root()+"/ui/src/pages", os.ModePerm); err != nil {
		fmt.Println(err)
		return
	}

	for k, v := range g.templates {
		tmpl, err := template.ParseFiles(g.Template(k))
		if err != nil {
			fmt.Println(err)
			continue
		}

		file, err := os.Create(g.g.config.Root() + "/ui/" + v)
		if err != nil {
			fmt.Println(err)
			continue
		}
		defer file.Close()

		tmpl.Execute(file, g.g.vars)
	}
}

func (g *UIGenerator) Template(t string) string {
	return g.g.Template("/ui/" + t)
}
