package apollo

import (
	"fmt"
	"os"

	"github.com/apolloconfig/agollo/v4"
	"github.com/apolloconfig/agollo/v4/env/config"
)

var client *agollo.Client
var c *config.AppConfig

func Start() {
	appid := os.Getenv("APOLLO_CONFIG_APPID")
	cluster := os.Getenv("APOLLO_CONFIG_CLUSTER")
	ip := os.Getenv("APOLLO_CONFIG_ADDR")
	ns := os.Getenv("APOLLO_CONFIG_NAMESPACE")

	c = &config.AppConfig{
		AppID:            appid,
		Cluster:          cluster,
		IP:               ip,
		NamespaceName:    ns,
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
	client = &cli
	fmt.Println("初始化Apollo配置成功")
}

func Client() *agollo.Client {
	return client
}

func GetString(ns, key string) string {
	cache := (*client).GetConfigCache(ns)
	value, _ := cache.Get(key)
	return value.(string)
}
