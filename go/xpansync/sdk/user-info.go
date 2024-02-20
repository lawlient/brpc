package sdk

import (
	"context"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"

	"xpansync/apollo"
	openapiclient "xpansync/openxpanapi"
)

func Userinfo(w http.ResponseWriter, req *http.Request) {
	accessToken := apollo.AccessToken()

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	_, r, err := api_client.UserinfoApi.Xpannasuinfo(context.Background()).AccessToken(accessToken).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `UserinfoApi.Xpannasuinfo``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(w, "err: %v\n", r)
		return
	}
	fmt.Fprintf(w, "%s\n", string(bodyBytes))
}

func Quota(w http.ResponseWriter, req *http.Request) {
	accessToken := apollo.AccessToken()
	checkexpire := int32(1) // int32 |  (optional)
	checkfree := int32(1)   // int32 |  (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	_, r, err := api_client.UserinfoApi.Apiquota(context.Background()).AccessToken(accessToken).Checkexpire(checkexpire).Checkfree(checkfree).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfilelist``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(w, "err: %v\n", r)
		return
	}
	fmt.Fprintf(w, "%s\n", string(bodyBytes))
}
