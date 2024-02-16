package sdk

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
)

type config struct {
	AppID          string `json:"AppID"`
	AppKey         string `json:"AppKey"`
	SecretKey      string `json:"SecretKey"`
	SignKey        string `json:"SignKey"`
	RedirectUri    string `json:"RedirectUri`
	Code           string `json:"Code"`
	AccessToken    string `json:"AccessToken"`
	UploadFilelist string `json:"UploadFilelist"`
	LogFilename    string `json:"LogFilename"`
}

func (sdk *Sdk) InitConfig(path string) error {
	buf, err := ioutil.ReadFile(path)
	if err != nil {
		fmt.Println(err)
		return err
	}

	config := &config{}
	err = json.Unmarshal(buf, config)
	if err != nil {
		fmt.Println(err)
		return err
	}

	sdk.Config = config

	return nil
}
