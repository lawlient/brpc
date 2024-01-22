package main

import (
	"fmt"
	"log"

	"database/sql"

	_ "modernc.org/sqlite"
)

func DBInit() {
	db := DB()

	sql := `CREATE TABLE IF NOT EXISTS user (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT,
		password TEXT
	);`

	_, err := db.Exec(sql)
	if err != nil {
		log.Writer().Write([]byte("create user fail"))
		fmt.Println("table user init fail")
	}

	fmt.Println("数据库初始化完成")
}

func DB() *sql.DB {
	db, err := sql.Open("sqlite", "./data/x.db")
	if err != nil {
		log.Writer().Write([]byte(err.Error()))
		return nil
	}
	return db
}

func GetUser(u *User) bool {
	sql := `SELECT id, name, password from user where name = ?;`
	rows, err := DB().Query(sql, u.Name)
	if err != nil {
		fmt.Println(err, " name:", u.Name)
		return false
	}

	var exist = false
	for rows.Next() {
		err = rows.Scan(&u.ID, &u.Name, &u.Password)
		if err != nil {
			continue
		}
		exist = true
		break
	}
	return exist
}

func AddUser(u User) error {
	sql := `INSERT into user ("name", "password") values (?, ?)`
	u.Password = SaltHash([]byte(u.Password)) // 加密存储

	stmt, _ := DB().Prepare(sql)
	res, err := stmt.Exec(u.Name, u.Password)
	if err != nil {
		return err
	}

	_, err = res.RowsAffected()
	if err != nil {
		return err
	}

	return nil
}
