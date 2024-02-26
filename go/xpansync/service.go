package main

import (
	"os"
	"path/filepath"
	"sync"
	"time"

	"github.com/apolloconfig/agollo/v4/storage"
	"github.com/robfig/cron"

	"xpansync/apollo"
	"xpansync/sdk"
	"xpansync/util"
	"xpansync/xlog"
)

type Service struct {
	c *cron.Cron
}

func NewService() (*Service, error) {
	service := &Service{}
	return service, nil
}

// UploadFiles配置项：指定待同步文件列表，结构参见配置
// 网盘文件统一存储于CloudRoot配置项指定的路径下
// case 1: path is file, type is origin ==> upload origin file
// case 2: path is file, type is package ==> upload tar.gz package
// case 3: path is folder, type is origin ==> upload folder recursively
// case 4: path is folder, type is package ==> upload tar.gz package
func (s *Service) cronUploadFiles() {
	notifymsg := ""
	for _, x := range *apollo.UploadFiles() {
		path := x.Path   // file or folder
		method := x.Type // origin or package

		st, err := os.Stat(path)
		if err != nil {
			xlog.Logger.Error("stat file error", "path", path, "errmsg", err.Error())
			notifymsg += path + " fail\n"
			continue
		}

		if path[0] != '/' {
			errmsg := path + " is not absolute path"
			xlog.Logger.Warn(errmsg)
			notifymsg += errmsg + "\n"
			continue
		}

		if st.IsDir() {
			if method == "origin" {
				upload3(path, &notifymsg)
				continue
			}

			if method == "package" {
				upload4(path, &notifymsg)
				continue
			}

			continue
		}

		if method == "origin" {
			upload1(path, &notifymsg)
			continue
		}

		if method == "package" {
			upload2(path, &notifymsg)
			continue
		}

		xlog.Logger.Error("config error", "path", path, "type", method)
	}

	ChanifyText(notifymsg)
}

func upload1(file string, notifymsg *string) {
	dsc := apollo.CloudRoot() + "/" + file
	err := sdk.FileUpload(file, dsc)
	if err != nil {
		xlog.Logger.Error(err.Error())
		*notifymsg += file + " fail\n"
	} else {
		xlog.Logger.Info("upload file success", "file", file)
		*notifymsg += file + " success\n"
	}
}

func upload2(file string, notifymsg *string) {
	archive := filepath.Dir(file) + "/xpansync-data.tar.gz"
	if _, err := os.Stat(archive); err == nil {
		xlog.Logger.Info("remove archive first", "archive", archive)
		os.Remove(archive)
	}

	if err := util.TargzPackage(archive, file); err != nil {
		*notifymsg += "tar " + file + "fail\n"
		return
	}
	defer os.Remove(archive)

	upload1(archive, notifymsg)
}

func upload3(folder string, notifymsg *string) {
	err := filepath.Walk(folder, func(path string, info os.FileInfo, err error) error {
		if !info.IsDir() {
			dsc := apollo.CloudRoot() + "/" + path
			err := sdk.FileUpload(path, dsc)
			return err
		}
		return nil
	})
	if err != nil {
		*notifymsg += folder + " fail"
	} else {
		*notifymsg += folder + " success"
	}
}

func upload4(folder string, notifymsg *string) {
	archive := folder + "/xpansync-data.tar.gz"
	if _, err := os.Stat(archive); err == nil {
		xlog.Logger.Info("remove archive first", "archive", archive)
		os.Remove(archive)
	}

	if err := util.TargzPackage(archive, folder); err != nil {
		*notifymsg += "tar " + archive + "fail\n"
		return
	}
	defer os.Remove(archive)

	upload1(archive, notifymsg)
}

func (s *Service) Run() {
	if apollo.Client() != nil {
		go addChangeListener(s)
	}

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
	if s.c != nil {
		s.c.Stop()
	}
	spec := apollo.UploadSpec()
	if len(spec) == 0 {
		xlog.Logger.Info("no cron spec")
		return
	}
	if _, err := cron.Parse(spec); err != nil {
		xlog.Logger.Error("parse spec error", "spec", spec)
		return
	}

	s.c = cron.New()
	s.c.AddFunc(spec, func() {
		s.cronUploadFiles()
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
	(*apollo.Client()).AddChangeListener(c2)
	c2.wg.Wait()
}
