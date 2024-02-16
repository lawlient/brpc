package xlog

import (
	"log"
	"os"
)

type Xloger struct {
	Logger *log.Logger
}

func (xlog *Xloger) Init(name string) {
	logfile, err := os.OpenFile(name, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0666)
	if err != nil {
		panic(err)
	}

	xlog.Logger = log.New(logfile, "", log.LstdFlags|log.Lshortfile)
}
