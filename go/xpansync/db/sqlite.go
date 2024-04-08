package db

import (
	"database/sql"
	"embed"
	"fmt"
	"strings"

	"xpansync/xlog"

	_ "modernc.org/sqlite" // sqlite driver
)

type sqlite struct {
	db *sql.DB
}

//go:embed sqlite.sql
var schema string

//go:embed sqlite.sql
var fschema embed.FS

func init() {
	drivers["sqlite"] = func(dsn string) (DB, error) {
		items := strings.Split(dsn, "://")
		path := items[1]
		db, _ := sql.Open(items[0], "file:"+path)
		if err := db.Ping(); err != nil {
			return nil, err
		}

		xlog.Logger.Info("open sqlite database.", "file", path)
		s := &sqlite{db: db}
		if err := s.initSchema(); err != nil {
			return nil, err
		}
		return s, nil
	}
	fmt.Println("dri size", len(drivers))
}

func (s *sqlite) initSchema() error {
	tx, err := s.db.Begin()
	if err != nil {
		xlog.Logger.Error("sqlite transacation begin fail.", "ErrMsg", err.Error())
		return err
	}
	defer tx.Rollback()

	if _, err := tx.Exec(string(schema)); err != nil {
		xlog.Logger.Error("sqlite init schema fail.", "ErrMsg", err.Error())
		return err
	}
	return tx.Commit()
}

func (s *sqlite) GetAllSetting() ([]Setting, error) {
	settings := make([]Setting, 0)
	sql := "select key, value, description from setting order by id"
	rows, err := s.db.Query(sql)
	if err != nil {
		xlog.Logger.Error("EXEC fail.", "sql", sql, "ErrMsg", err.Error())
		return nil, err
	}
	defer rows.Close()

	for rows.Next() {
		set := Setting{}
		if err := rows.Scan(&set.Key, &set.Value, &set.Description); err != nil {
			xlog.Logger.Warn("scan row fail.", "ErrMsg", err)
			continue
		}
		settings = append(settings, set)
	}
	return settings, nil
}

func (s *sqlite) GetSetting(key string) (*string, error) {
	var value string
	sql := "select value from setting where key = ?"
	if err := s.db.QueryRow(sql, key).Scan(&value); err != nil {
		return nil, err
	}
	return &value, nil
}

func (s *sqlite) SetSetting(settings []Setting) error {
	for _, set := range settings {
		if _, err := s.db.Exec("update setting set value = ? where key = ?", set.Value, set.Key); err != nil {
			return err
		} else {
			xlog.Logger.Info("update setting success.", "key", set.Key, "value", set.Value)
		}
	}
	return nil
}

func (s *sqlite) GetAllTask() ([]Task, error) {
	sql := "select id, ff, how from task order by id"
	rows, err := s.db.Query(sql)
	if err != nil {
		return nil, err
	}

	tasks := make([]Task, 0)
	for rows.Next() {
		var t Task
		if err := rows.Scan(&t.ID, &t.FF, &t.How); err != nil {
			xlog.Logger.Error("scan row data error.", "ErrMsg", err.Error())
			continue
		}
		tasks = append(tasks, t)
	}
	return tasks, nil
}

func (s *sqlite) DelTask(t *Task) error {
	_, err := s.db.Exec("delete from task where id = ?", t.ID)
	return err
}

func (s *sqlite) UpsertTask(t *Task) error {
	sql := `insert into task (ff, how) values (?, ?)
	on conflict(ff) do update set ff = ?, how = ? where id = ?`

	_, err := s.db.Exec(sql, t.FF, t.How, t.FF, t.How, t.ID)
	return err
}

func (s *sqlite) Close() {
	if s.db != nil {
		s.db.Close()
		s.db = nil
		xlog.Logger.Info("Close sqlite database")
	}
}
