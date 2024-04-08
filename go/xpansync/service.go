package main

import (
	"net/http"
	"os"
	"path/filepath"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/robfig/cron"

	"xpansync/db"
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

	for _, x := range sdk.Tasklist() {
		path := x.FF    // file or folder
		method := x.How // origin or package

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
	dsc := sdk.GetCloudRoot() + "/" + file
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
			dsc := sdk.GetCloudRoot() + "/" + path
			err := sdk.FileUpload(path, dsc)
			return err
		}
		return nil
	})
	if err != nil {
		*notifymsg += folder + " fail\n"
	} else {
		*notifymsg += folder + " success\n"
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
	spec := sdk.GetCron()
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

func Setting(c *gin.Context) {
	setting, err := db.INS.GetAllSetting()
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}
	c.JSON(http.StatusOK, gin.H{
		"data": setting,
	})

}

func SetSetting(c *gin.Context) {
	var set db.Setting
	if err := c.ShouldBindJSON(&set); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{})
	}
	// reset cron
	settings := []db.Setting{set}
	err := db.INS.SetSetting(settings)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"msg": "OK",
	})
}
