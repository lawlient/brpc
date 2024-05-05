package main

import (
	"embed"
	"fmt"
	"io/fs"
	"log"
	"net/http"
	"os"
	"strings"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/spf13/afero"
)

//go:embed dist
var ui embed.FS

//go:embed dist/index.html
var index string

// vite构建前端页面时，通过设置base为__BASEURL__变量，再通过后端运行时通过系统变量进行宏替换
// 目的是实现：运行时环境变量控制前端基础路径
// 由于embed是编译时确定的，无法直接修改原文件系统，此处通过启动时宏替换构建内存文件系统
func uiMemFS() http.FileSystem {
	dirFS, err := fs.Sub(ui, "dist")
	if err != nil {
		log.Fatal(err)
	}

	memFS := afero.NewMemMapFs()
	uiFS := afero.NewBasePathFs(memFS, "/")
	baseurl := os.Getenv("BASEURL")
	err = fs.WalkDir(dirFS, ".", func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			fmt.Println(err)
			return err
		}
		if d.IsDir() {
			return nil
		}

		data, err := fs.ReadFile(dirFS, path)
		if err != nil {
			return err
		}

		replaced := strings.ReplaceAll(string(data), "/__BASEURL__", baseurl)

		file, _ := uiFS.Create(path)
		file.Write([]byte(replaced))
		defer file.Close()
		return nil
	})

	if err != nil {
		log.Fatal(err)
		return nil
	}

	httpfs := afero.NewHttpFs(uiFS)
	return httpfs
}

func embedFrontend() gin.HandlerFunc {
	uifs := uiMemFS()
	fileserver := http.FileServer(uifs)

	return func(c *gin.Context) {
		path := c.Request.URL.Path
		fmt.Println(path)

		// server
		if strings.HasPrefix(path, "/api") || strings.HasPrefix(path, "/auth") {
			return
		}

		// ui相关
		_, err := uifs.Open(path)
		if err == nil {
			// 静态资源文件
			fileserver.ServeHTTP(c.Writer, c.Request)
			return
		} else {
			fmt.Println(err)
		}

		// SPA入口文件
		file, err := uifs.Open("index.html")
		if err != nil {
			// 不存在视为异常
			fmt.Println(err)
			return
		}

		http.ServeContent(c.Writer, c.Request, "index.html", time.Now(), file)
	}
}
