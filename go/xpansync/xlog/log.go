package xlog

import (
	"log/slog"
	"os"
	"strconv"
	"time"

	"xpansync/util"
)

var Logger *slog.Logger

const logfilename = "/var/lib/xpansync/main.log"

func init() {
	reset()
}

func LogRotate() {
	info, err := os.Stat(logfilename)
	if err != nil {
		reset()
		return
	}

	if info.Size() > 4*1024*1024 {
		now := strconv.FormatInt(time.Now().Unix(), 10)
		err = os.Rename(logfilename, logfilename+"."+now)
		if err == nil {
			reset()
			return
		}

		Logger.Error("rename log file fail.", "error", err.Error())
		return
	}
	Logger.Info("logger works fine.", "size", util.Byte2IEC(info.Size()))
}

func reset() {
	logfile, err := os.OpenFile(logfilename, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0666)
	if err != nil {
		logfile = os.Stderr
	}

	Logger = slog.New(slog.NewJSONHandler(logfile, &slog.HandlerOptions{
		AddSource: true,
		Level:     slog.LevelDebug,
	}))

}
