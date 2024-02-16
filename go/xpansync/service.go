package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
	"time"
	"xpansync/sdk"
)

type Service struct {
	sdk *sdk.Sdk
}

func NewService() (*Service, error) {
	sdk, err := sdk.NewSdk(".appinfo.json")
	if err != nil {
		return nil, err
	}

	service := &Service{}
	service.sdk = sdk
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
		fmt.Println(err)
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
			fmt.Fprintf(os.Stderr, "can not recognize %s\n", src)
			continue
		}
		dsc := strings.Replace(src[1:], "/", "0x27", -1 /*all*/)
		if len(dsc) == 0 {
			fmt.Fprintf(os.Stderr, "src:%s dsc:%s\n", src, dsc)
			continue
		}
		s.sdk.FileUpload(src, "/jarvis/"+dsc)
	}

	return nil
}

func (s *Service) Run() {
	ticker := time.NewTicker(5 * time.Minute)
	defer ticker.Stop()
	alive := time.NewTicker(3 * time.Second)
	defer alive.Stop()

	for {
		select {
		case <-ticker.C:
			s.UploadFiles()
		case <-alive.C:
			fmt.Fprintf(os.Stdout, "I am running, %s\n", time.Now().Format("2006-01-02 15:04:05"))
		}
	}

}
