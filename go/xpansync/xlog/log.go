package xlog

import (
	"fmt"
	"log/slog"
	"os"
	"strconv"
	"time"
)

var Logger *slog.Logger

const logfilename = "/var/lib/xpansync/main.log"

func init() {
	fmt.Println("log init")
	reset()
}

func LogRotate() {
	info, err := os.Stat(logfilename)
	if err != nil {
		reset()
		return
	}

	if info.Size() > 4000000 {
		now := strconv.FormatInt(time.Now().Unix(), 10)
		err = os.Rename(logfilename, logfilename+"."+now)
		if err == nil {
			reset()
			return
		}

		Logger.Error("rename log file fail.", "error", err.Error())
		return
	}
}

func reset() {
	logfile, err := os.OpenFile(logfilename, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0666)
	if err != nil {
		logfile = os.Stdout
	}

	Logger = slog.New(slog.NewJSONHandler(logfile, &slog.HandlerOptions{
		AddSource: true,
		Level:     slog.LevelDebug,
	}))

}
