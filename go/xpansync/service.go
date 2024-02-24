package main

import (
	"bufio"
	"os"
	"strings"
	"sync"
	"time"

	"github.com/apolloconfig/agollo/v4/storage"
	"github.com/robfig/cron"

	"xpansync/apollo"
	"xpansync/sdk"
	"xpansync/xlog"
)

type Service struct {
	c *cron.Cron
}

func NewService() (*Service, error) {
	service := &Service{}
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
	nofifymsg := ""
	for scanner.Scan() {
		src := scanner.Text()
		if len(src) == 0 {
			continue
		}
		if src[0] == '~' {
			src = strings.Replace(src, "~", HOME, 1)
		}
		if src[0] != '/' {
			xlog.Logger.Warn("can not recognize " + src)
			continue
		}
		dsc := strings.Replace(src[1:], "/", "0x27", -1 /*all*/)
		if len(dsc) == 0 {
			xlog.Logger.Info("src:", src, " dsc:", dsc)
			continue
		}
		err := sdk.FileUpload(src, "/jarvis/"+dsc)
		if err != nil {
			xlog.Logger.Error(err.Error())
		} else {
			xlog.Logger.Info("upload ", src, " success.")
			nofifymsg += "- [x] " + src + "\n"
		}
	}

	ChanifyText(nofifymsg)
	return nil
}

func (s *Service) Run() {
	go addChangeListener(s)

	s.ResetCron()

	alive := time.NewTicker(3 * time.Second)
	defer alive.Stop()
	for {
		select {
		case <-alive.C:
			xlog.LogRotate()
		}
	}

}

func (s *Service) ResetCron() {
	spec := apollo.UploadSpec()
	if _, err := cron.Parse(spec); err != nil {
		xlog.Logger.Error("parse spec error", "spec", spec)
		return
	}
	if s.c != nil {
		s.c.Stop()
	}
	s.c = cron.New()

	s.c.AddFunc(spec, func() {
		s.uploadFiles()
	})
	s.c.Start()

	entrysize := len(s.c.Entries())
	xlog.Logger.Info("register cron.", "spec", spec, "entrysize", entrysize)
}

// Listen apollo config changed event
type CustomChangeListener struct {
	wg      sync.WaitGroup
	service *Service
}

func (c *CustomChangeListener) OnChange(ev *storage.ChangeEvent) {
	//write your code here
	for key, value := range ev.Changes {
		xlog.Logger.Info("apollo config changed", "key", key, "value", value)
		if key == "UploadSpec" {
			c.service.ResetCron()
		}
	}
	c.wg.Done()
}

func (c *CustomChangeListener) OnNewestChange(event *storage.FullChangeEvent) {
	//write your code here
}

func addChangeListener(s *Service) {
	c2 := &CustomChangeListener{}
	c2.wg.Add(5)
	c2.service = s
	apollo.Client().AddChangeListener(c2)
	c2.wg.Wait()
}
