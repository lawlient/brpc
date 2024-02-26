package apollo

import (
	"encoding/json"
	"xpansync/xlog"
)

type Application struct {
	AppID       string `json:"AppID"`
	AppKey      string `json:"AppKey"`
	SecretKey   string `json:"SecretKey"`
	SignKey     string `json:"SignKey"`
	RedirectUri string `json:"RedirectUri`
	AccessToken string `json:"AccessToken"`
}

const ns = "application"

func AppID() string {
	return GetString(ns, "AppID")
}

func AppKey() string {
	return GetString(ns, "AppKey")
}

func SecretKey() string {
	return GetString(ns, "SecretKey")
}

func SignKey() string {
	return GetString(ns, "SignKey")
}

func AccessToken() string {
	return GetString(ns, "AccessToken")
}

func RedirectUri() string {
	return GetString(ns, "RedirectUri")
}

func CloudRoot() string {
	return GetString(ns, "CloudRoot")
}

// json array
// [{}]
type SyncFileConf struct {
	Path string `json:"path"`
	Type string `json:"type"` // origin, package
}

func UploadFiles() *[]SyncFileConf {
	field := GetString(ns, "UploadFiles")
	res := make([]SyncFileConf, 0)
	err := json.Unmarshal([]byte(field), &res)
	if err != nil {
		xlog.Logger.Error("parse UploadFiles fail", "errmsg", err.Error())
		return nil
	}
	return &res
}

func UploadSpec() string {
	return GetString(ns, "UploadSpec")
}

func ChanifyAddr() string {
	return GetString(ns, "ChanifyAddr")
}
