package sdk

import (
	"context"
	"fmt"
	"net/http"
	"os"

	openapiclient "xpansync/openxpanapi"

	"github.com/gin-gonic/gin"
)

func Userinfo(c *gin.Context) {
	accessToken := getToken()

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.UserinfoApi.Xpannasuinfo(context.Background()).AccessToken(accessToken).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `UserinfoApi.Xpannasuinfo``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	c.JSON(http.StatusOK, gin.H{
		"data": resp,
	})
}

func Quota(c *gin.Context) {
	accessToken := getToken()
	checkexpire := int32(1) // int32 |  (optional)
	checkfree := int32(1)   // int32 |  (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.UserinfoApi.Apiquota(context.Background()).AccessToken(accessToken).Checkexpire(checkexpire).Checkfree(checkfree).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfilelist``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	c.JSON(http.StatusOK, gin.H{
		"data": resp,
	})
}
