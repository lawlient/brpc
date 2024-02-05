package sdk

import (
	"context"
	"fmt"
	"os"

	openapiclient "xpansync/openxpanapi"
)

func (sdk *sdk) Userinfo() (*openapiclient.Uinforesponse, error) {
	accessToken := sdk.config.AccessToken // string

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	uinfo, r, err := api_client.UserinfoApi.Xpannasuinfo(context.Background()).AccessToken(accessToken).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `UserinfoApi.Xpannasuinfo``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	return &uinfo, nil
}

func (sdk *sdk) Quota() (*openapiclient.Quotaresponse, error) {
	accessToken := sdk.config.AccessToken // string
	checkexpire := int32(1)               // int32 |  (optional)
	checkfree := int32(1)                 // int32 |  (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.UserinfoApi.Apiquota(context.Background()).AccessToken(accessToken).Checkexpire(checkexpire).Checkfree(checkfree).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfilelist``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	return &resp, nil
}
