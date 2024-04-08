package sdk

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"

	"xpansync/db"
	openapiclient "xpansync/openxpanapi"

	"github.com/gin-gonic/gin"
)

const hostname = "https://openapi.baidu.com/oauth/2.0"

type authReturnType struct {
	ExpiresIn     int    `json:"expires_in"`
	RefreshToken  string `json:"refresh_token"`
	AccessToken   string `json:"access_token"`
	SessionSecret string `json:"session_secret"`
	SessionKey    string `json:"session_key"`
	Scope         string `json:"scope"`
}

func GetAccessCode(c *gin.Context) {
	api := hostname + "/authorize?"
	args := []string{}
	args = append(args, "response_type=code")
	args = append(args, "client_id="+getAppKey())
	args = append(args, "redirect_uri="+"oob")
	args = append(args, "scope=basic,netdisk")
	args = append(args, "device_id="+getAppID())

	url := api + strings.Join(args, "&")
	c.JSON(http.StatusOK, gin.H{
		"data": url,
	})
}

func MyOauthTokenAuthorizationCode(c *gin.Context) {
	var reqbody struct {
		Code string
	}
	if err := c.ShouldBindJSON(&reqbody); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{})
		return
	}
	clientId := getAppKey()
	clientSecret := getSecretKey()
	redirectUri := "oob"

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.AuthApi.OauthTokenCode2token(context.Background()).Code(reqbody.Code).ClientId(clientId).ClientSecret(clientSecret).RedirectUri(redirectUri).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `AuthApi.OauthTokenCode2token``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}
	// response from `OauthTokenCode2token`: OauthTokenAuthorizationCodeResponse
	fmt.Fprintf(os.Stdout, "Response from `AuthApi.OauthTokenCode2token`: %v\n", resp)

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "err: %v\n", r)
	}

	p := &authReturnType{}
	if err := json.Unmarshal(bodyBytes, p); err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}

	var setting db.Setting
	setting.Key = "AccessToken"
	setting.Value = p.AccessToken
	err = db.INS.SetSetting([]db.Setting{setting})
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}
	c.JSON(http.StatusOK, gin.H{
		"data": p.AccessToken,
	})
}

func GetCurrentToken(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{
		"data": getToken(),
	})
}
