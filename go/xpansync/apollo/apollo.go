package apollo

import (
	"fmt"

	"github.com/apolloconfig/agollo/v4"
	"github.com/apolloconfig/agollo/v4/env/config"
)

var client agollo.Client
var c *config.AppConfig

func Start() {
	c = &config.AppConfig{
		AppID:            "xpansync",
		Cluster:          "pro",
		IP:               "http://apollo-pro-config-service:8080",
		NamespaceName:    "application",
		IsBackupConfig:   true,
		BackupConfigPath: "/var/lib/xpansync/.config",
		Secret:           "",
	}

	cli, err := agollo.StartWithConfig(func() (*config.AppConfig, error) {
		return c, nil
	})
	if err != nil {
		fmt.Println(err)
		return
	}
	client = cli
	fmt.Println("初始化Apollo配置成功")
}

func Client() agollo.Client {
	return client
}

func GetString(ns, key string) string {
	cache := client.GetConfigCache(ns)
	value, _ := cache.Get(key)
	return value.(string)
}
