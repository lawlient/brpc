package db

import (
	"errors"
	"strings"
)

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

type Role string

var (
	USER  Role = "user"
	ROOT  Role = "root"
	ADMIN Role = "admin"
)

type User struct {
	ID       int    `json:"id"`
	Username string `json:"username"`
	Password string `json:"password"`
	Avatar   string `json:"avatar"`
	Role     Role   `json:"role"`
}

type DB interface {
	GetAllSetting() ([]Setting, error)
	GetSetting(key string) (*string, error)
	SetSetting([]Setting) error

	GetAllTask() ([]Task, error)
	DelTask(t *Task) error
	UpsertTask(t *Task) error

	GetUser(u *User) ([]*User, error)
	AddUser(u *User) (*User, error)
	ModUser(u *User) (*User, error)
	DelUser(id int) error
	UserCount() (int, error)

	Close()
}

type OpenDB func(dsn string) (DB, error)

var (
	drivers = map[string]OpenDB{}

	ErrDriverNotFound = errors.New("driver not found")
	ErrNotImplemented = errors.New("not implemented")
	ErrInvalidDSN     = errors.New("invalid dsn")
)

func InitDB(dsn string) (DB, error) {
	items := strings.Split(dsn, "://")
	if len(items) <= 1 {
		return nil, ErrInvalidDSN
	}
	dbOpen, ok := drivers[strings.ToLower(items[0])]
	if !ok {
		return nil, ErrDriverNotFound
	}
	return dbOpen(dsn)
}
