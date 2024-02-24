package main

import (
	"fmt"
	"net/http"

	"xpansync/apollo"
	"xpansync/sdk"
	"xpansync/xlog"
)

func main() {
	service, err := NewService()
	if err != nil {
		fmt.Println(err)
		return
	}

	// 文件列表
	// sdk.FileImageList()
	// sdk.FileDocList()
	// sdk.FileSearch()
	// sdk.FileList()

	apollo.Start()

	go service.Run()

	http.HandleFunc("/", hello)
	http.HandleFunc("/code", sdk.GetAccessCode)
	http.HandleFunc("/token", sdk.MyOauthTokenAuthorizationCode)
	http.HandleFunc("/user", sdk.Userinfo)
	http.HandleFunc("/quota", sdk.Quota)
	if err := http.ListenAndServe(":1425", nil); err != nil {
		xlog.Logger.Error("start http server fail, err:", err)
	}
}

func hello(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hello, World!")
}
