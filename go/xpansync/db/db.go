package db

import (
	"errors"
	"fmt"
	"strings"
)

var INS DB

type Setting struct {
	Key         string `json:"Key"`
	Value       string `json:"Value"`
	Description string `json:"Description"`
}

type Task struct {
	ID  int    `json:"id"`
	FF  string `json:"ff"`
	How string `json:"how"`
}

type DB interface {
	GetAllSetting() ([]Setting, error)
	GetSetting(key string) (*string, error)
	SetSetting([]Setting) error

	GetAllTask() ([]Task, error)
	DelTask(t *Task) error
	UpsertTask(t *Task) error

	Close()
}

type OpenDB func(dsn string) (DB, error)

var (
	drivers = map[string]OpenDB{}

	ErrDriverNotFound   = errors.New("driver not found")
	ErrNotImplemented   = errors.New("not implemented")
	ErrInvalidDeviceID  = errors.New("invalid device id")
	ErrInvalidUserID    = errors.New("invalid user id")
	ErrInvalidPublicKey = errors.New("invalid public key")
	ErrInvalidToken     = errors.New("invalid token")
	ErrInvalidDSN       = errors.New("invalid dsn")
)

func InitDB(dsn string) (DB, error) {
	items := strings.Split(dsn, "://")
	if len(items) <= 1 {
		return nil, ErrInvalidDSN
	}
	fmt.Println("driv size", len(drivers))
	dbOpen, ok := drivers[strings.ToLower(items[0])]
	if !ok {
		return nil, ErrDriverNotFound
	}
	return dbOpen(dsn)
}
