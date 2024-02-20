package main

import (
	"bufio"
	"log"
	"os"
	"strings"
	"time"

	"github.com/robfig/cron"

	"xpansync/apollo"
	"xpansync/sdk"
	"xpansync/xlog"
)

type Service struct {
	Logger *xlog.Xloger
}

func NewService() (*Service, error) {
	service := &Service{}
	service.Logger = &xlog.Xloger{}
	service.Logger.Init("/var/lib/xpansync/main.log")
	return service, nil
}

// 配置项：UploadFiles指定待同步文件列表
// 格式： 每行为系统单一文件名，绝对路径或环境变量~开头
// 注意：不支持相对路径, 包括basename
// 网盘文件统一存储于特定目录下，无层级结构
// 如：/var/lib/mysql/data.sql  -->  var0x27lib0x27mysql0x27data.sql
func (s *Service) uploadFiles() error {
	scanner := bufio.NewScanner(strings.NewReader(apollo.UploadFiles()))
	HOME := os.Getenv("HOME")
	for scanner.Scan() {
		src := scanner.Text()
		if len(src) == 0 {
			continue
		}
		if src[0] == '~' {
			src = strings.Replace(src, "~", HOME, 1)
		}
		if src[0] != '/' {
			s.Log().Println("can not recognize " + src)
			continue
		}
		dsc := strings.Replace(src[1:], "/", "0x27", -1 /*all*/)
		if len(dsc) == 0 {
			s.Log().Println("src:", src, " dsc:", dsc)
			continue
		}
		err := sdk.FileUpload(src, "/jarvis/"+dsc)
		if err != nil {
			s.Log().Println(err)
		} else {
			s.Log().Println("upload ", src, " success.")
		}
	}

	return nil
}

func (s *Service) Run() {
	c := cron.New()
	spec := apollo.UploadSpec()
	c.AddFunc(spec, func() {
		s.uploadFiles()
	})
	c.Start()
	s.Log().Println(spec)

	alive := time.NewTicker(3 * time.Second)
	defer alive.Stop()
	for {
		select {
		case <-alive.C:
			s.Log().Println("I am running")
		}
	}

}

func (s *Service) Log() *log.Logger {
	return s.Logger.Logger
}
