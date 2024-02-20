package sdk

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"

	"xpansync/apollo"
	openapiclient "xpansync/openxpanapi"
)

const hostname = "http://openapi.baidu.com/oauth/2.0"

type authReturnType struct {
	ExpiresIn     int    `json:"expires_in"`
	RefreshToken  string `json:"refresh_token"`
	AccessToken   string `json:"access_token"`
	SessionSecret string `json:"session_secret"`
	SessionKey    string `json:"session_key"`
	Scope         string `json:"scope"`
}

func GetAccessCode(w http.ResponseWriter, r *http.Request) {
	api := hostname + "/authorize?"
	args := []string{}
	args = append(args, "response_type=code")
	args = append(args, "client_id="+apollo.AppKey())
	args = append(args, "redirect_uri="+apollo.RedirectUri())
	args = append(args, "scope=basic,netdisk")
	args = append(args, "device_id="+apollo.AppID())

	url := api + strings.Join(args, "&")
	resp, err := http.Get(url)
	if err != nil {
		fmt.Fprintf(w, "%s", err.Error())
		return
	}

	defer resp.Body.Close()
	_, err = ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Fprintf(w, "%s", err.Error())
		return
	}
	fmt.Fprintf(w, "%s\n", url)
}

func MyOauthTokenAuthorizationCode(w http.ResponseWriter, req *http.Request) {
	code := req.Header.Get("Code")
	clientId := apollo.AppKey()
	clientSecret := apollo.SecretKey()
	redirectUri := apollo.RedirectUri()

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.AuthApi.OauthTokenCode2token(context.Background()).Code(code).ClientId(clientId).ClientSecret(clientSecret).RedirectUri(redirectUri).Execute()
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
		fmt.Fprintf(w, "%s", err.Error())
		return
	}

	fmt.Fprintf(w, "%s\n", p.AccessToken)
}
