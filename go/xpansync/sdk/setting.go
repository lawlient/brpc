package sdk

import (
	"xpansync/db"
)

func getsetting(key string) string {
	value, err := db.INS.GetSetting(key)
	if err != nil {
		return ""
	}
	return *value

}

func getToken() string { return getsetting("AccessToken") }

func getAppKey() string { return getsetting("AppKey") }

func getAppID() string { return getsetting("AppID") }

func getSecretKey() string { return getsetting("SecretKey") }

func GetChanifyAddr() string { return getsetting("ChanifyAddr") }

func GetCloudRoot() string { return getsetting("CloudRoot") }

func GetCron() string { return getsetting("Cron") }

func Tasklist() []db.Task {
	tasks := make([]db.Task, 0)
	list, err := db.INS.GetAllTask()
	if err == nil {
		tasks = append(tasks, list...)
	}
	return tasks
}
