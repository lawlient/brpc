package main

import (
	"fmt"
	"net/http"
	"os"

	"xpansync/db"
	"xpansync/sdk"
	"xpansync/xlog"

	"github.com/gin-gonic/gin"
)

func CORS() gin.HandlerFunc {
	return func(c *gin.Context) {
		c.Writer.Header().Set("Access-Control-Allow-Origin", "*")
		c.Writer.Header().Set("Access-Control-Allow-Credentials", "true")
		c.Writer.Header().Set("Access-Control-Allow-Headers", "Content-Type, Content-Length, Accept-Encoding, X-CSRF-Token, Authorization, accept, origin, Cache-Control, X-Requested-With")
		c.Writer.Header().Set("Access-Control-Allow-Methods", "POST, OPTIONS, GET, PUT")

		if c.Request.Method == "OPTIONS" {
			c.AbortWithStatus(204)
			return
		}

		c.Next()
	}
}

func main() {
	os.MkdirAll("/var/lib/xpansync", os.ModePerm)

	service, err := NewService()
	if err != nil {
		fmt.Println(err)
		return
	}

	db.INS, err = db.InitDB("sqlite:///var/lib/xpansync/setting.db")
	if err != nil {
		fmt.Println(err)
		return
	}

	go service.Run()

	http.HandleFunc("/", hello)
	e := gin.Default()
	e.Use(CORS())
	{
		e.POST("/token/update", sdk.MyOauthTokenAuthorizationCode)
		e.GET("/token", sdk.GetCurrentToken)
		e.POST("/code", sdk.GetAccessCode)
		e.POST("/user", sdk.Userinfo)
		e.POST("/quota", sdk.Quota)
		e.POST("/setting", Setting)
		e.POST("/setting/update", SetSetting)

		e.POST("/tasks", sdk.GetAllTask)
		e.POST("/tasks/add", sdk.AddTask)
		e.POST("/tasks/del", sdk.DelTask)
		e.POST("/tasks/upd", sdk.UpdateTask)
	}

	http.HandleFunc("/filelist", sdk.Filelist)
	http.HandleFunc("/upload", sdk.Upload)
	if err := e.Run(":1425"); err != nil {
		xlog.Logger.Error("start http server fail, err:", err)
	}
}

func hello(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hello, World!")
}
