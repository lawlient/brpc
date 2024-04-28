package db

import (
	"database/sql"
	"embed"
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

func (s *sqlite) Close() {
	if s.db != nil {
		s.db.Close()
		s.db = nil
		xlog.Logger.Info("Close sqlite database")
	}
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

func (s *sqlite) GetUser(u *User) ([]*User, error) {
	where := []string{"1 = 1"}
	args := []any{}

	if u.ID != 0 {
		where, args = append(where, "id = ?"), append(args, u.ID)
	}
	if u.Username != "" {
		where, args = append(where, "username = ?"), append(args, u.Username)
	}

	sql := `
        select id, username, passwd, avatar, role
        from users where ` + strings.Join(where, " and ") + `
        order by id desc
    `

	rows, err := s.db.Query(sql, args...)
	if err != nil {
		xlog.Logger.Error("SQL ERROR", "sql", sql, "Errmsg", err.Error())
		return nil, err
	}

	list := make([]*User, 0)
	for rows.Next() {
		var tmp User
		if err := rows.Scan(
			&tmp.ID,
			&tmp.Username,
			&tmp.Password,
			&tmp.Avatar,
			&tmp.Role,
		); err != nil {
			xlog.Logger.Error("Scan row error", "ErrMsg", err)
			return nil, err
		}

		list = append(list, &tmp)
	}
	return list, nil
}

func (s *sqlite) AddUser(u *User) (*User, error) {
	fields := []string{"username", "passwd", "avatar", "role"}
	placeholders := []string{"?", "?", "?", "?"}
	args := []any{u.Username, u.Password, u.Avatar, u.Role}

	sql := `insert into users (` + strings.Join(fields, ", ") + `) values (` + strings.Join(placeholders, ",") + `)
    RETURNING id, username, passwd, avatar, role`

	if err := s.db.QueryRow(sql, args...).Scan(
		&u.ID,
		&u.Username,
		&u.Password,
		&u.Avatar,
		&u.Role,
	); err != nil {
		xlog.Logger.Error("SQL EXEC ERROR", "sql", sql, "ErrMsg", err)
		return nil, err
	}
	return u, nil
}

func (s *sqlite) ModUser(u *User) (*User, error) {
	fields := []string{}
	args := []any{}

	if u.Username != "" {
		fields, args = append(fields, "username = ?"), append(args, u.Username)
	}
	// if u.Password != "" {
	// 	fields, args = append(fields, "passwd = ?"), append(args, u.Password)
	// }
	if u.Avatar != "" {
		fields, args = append(fields, "avatar = ?"), append(args, u.Avatar)
	}
	if u.Role != "" {
		fields, args = append(fields, "role = ?"), append(args, u.Role)
	}

	sql := `
        update users set ` + strings.Join(fields, ", ") + `
        where id = ?
        RETURNING id, username, passwd, avatar, role
    `
	args = append(args, u.ID)

	if err := s.db.QueryRow(sql, args...).Scan(
		&u.ID,
		&u.Username,
		&u.Password,
		&u.Avatar,
		&u.Role,
	); err != nil {
		xlog.Logger.Error("SQL Error", "sql", sql, "ErrMsg", err)
		return nil, err
	}
	return u, nil
}

func (s *sqlite) DelUser(uid int) error {
	sql := "delete from users where id = ?"
	affect, err := s.db.Exec(sql, uid)
	if err != nil {
		xlog.Logger.Error("SQL ERROR", "sql", sql, "ErrMsg", err)
		return err
	}

	if _, err := affect.RowsAffected(); err != nil {
		return err
	}

	return nil
}

func (s *sqlite) UserCount() (int, error) {
	sql := "select count(*) from users"
	var size = 0
	if err := s.db.QueryRow(sql).Scan(
		&size,
	); err != nil {
		xlog.Logger.Error("EXEC ERROR", "sql", sql, "ErrMsg", err)
		return 0, err
	}

	return size, nil
}
