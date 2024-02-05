package sdk

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"strings"
)

const (
	hostname = "http://openapi.baidu.com/oauth/2.0"
)

func (sdk *sdk) getAccessCode() (string, error) {
	api := hostname + "/authorize?"
	args := []string{}
	args = append(args, "response_type=code")
	args = append(args, "client_id="+sdk.config.AppKey)
	args = append(args, "redirect_uri="+sdk.config.RedirectUri)
	args = append(args, "scope=basic,netdisk")
	args = append(args, "device_id="+sdk.config.AppID)

	url := api + strings.Join(args, "&")
	fmt.Println(url)
	resp, err := http.Get(url)
	if err != nil {
		fmt.Println(err)
		return "", err
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println(err)
		return "", err
	}
	if resp.StatusCode == 200 {
		fmt.Println(string(body))
	}
	return string(body), nil
}

func (sdk *sdk) getAccessToken(code string) (string, error) {
	api := hostname + "/token?"
	args := []string{}

	args = append(args, "grant_type=authorization_code")
	args = append(args, "code="+code)
	args = append(args, "client_id="+sdk.config.AppKey)
	args = append(args, "client_secret="+sdk.config.SecretKey)
	args = append(args, "redirect_uri="+sdk.config.RedirectUri)

	url := api + strings.Join(args, "&")
	fmt.Println(url)
	resp, err := http.Get(url)
	if err != nil {
		fmt.Println(err)
		return "", err
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println(err)
		return "", err
	}
	if resp.StatusCode == 200 {
		fmt.Println(string(body))
	}
	return string(body), nil
}
