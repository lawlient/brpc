package main

import (
	"bufio"
	"log"
	"os"
	"strings"
	"time"
	"xpansync/sdk"
	"xpansync/xlog"
)

type Service struct {
	sdk    *sdk.Sdk
	Logger *xlog.Xloger
}

func NewService() (*Service, error) {
	sdk, err := sdk.NewSdk(".appinfo.json")
	if err != nil {
		return nil, err
	}

	service := &Service{}
	service.sdk = sdk
	service.Logger = &xlog.Xloger{}
	service.Logger.Init(service.sdk.Config.LogFilename)
	return service, nil
}

// 配置项：UploadFilelist指定待同步文件列表
// 格式： 每行为系统单一文件名，绝对路径或环境变量~开头
// 注意：不支持相对路径, 包括basename
// 网盘文件统一存储于特定目录下，无层级结构
// 如：/var/lib/mysql/data.sql  -->  var0x27lib0x27mysql0x27data.sql
func (s *Service) UploadFiles() error {
	name := s.sdk.Config.UploadFilelist
	file, err := os.Open(name)
	if err != nil {
		s.Log().Println(err)
		return err
	}

	defer file.Close()

	scanner := bufio.NewScanner(file)
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
		err := s.sdk.FileUpload(src, "/jarvis/"+dsc)
		if err != nil {
			s.Log().Println(err)
		} else {
			s.Log().Println("upload ", src, " success.")
		}
	}

	return nil
}

func (s *Service) Run() {
	uploader := time.NewTicker(12 * time.Hour)
	defer uploader.Stop()
	alive := time.NewTicker(3 * time.Second)
	defer alive.Stop()

	for {
		select {
		case <-uploader.C:
			s.UploadFiles()
		case <-alive.C:
			s.Log().Println("I am running")
		}
	}
}

func (s *Service) Log() *log.Logger {
	return s.Logger.Logger
}
