package sdk

import (
	"context"
	"fmt"
	"io/ioutil"
	"os"
	openapiclient "xpansync/openxpanapi"

	"xpansync/apollo"
)

func FileImageList() {
	accessToken := apollo.AccessToken()
	web := "1"        // string |  (optional)
	parentPath := "/" // string |  (optional)
	recursion := "1"  // string |  (optional)
	page := int32(1)  // int32 |  (optional)
	num := int32(1)   // int32 |  (optional)
	order := "time"   // string |  (optional)
	desc := "1"       // string |  (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfileimagelist(context.Background()).AccessToken(accessToken).ParentPath(parentPath).Recursion(recursion).Page(page).Num(num).Order(order).Desc(desc).Web(web).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfileimagelist``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "err: %v\n", r)
	}
	bodyString := string(bodyBytes)

	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfileimagelist`: %v body: %v\n", r, bodyString)
	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfileimagelist`: %v\n", resp)
}

func FileDocList() {
	accessToken := apollo.AccessToken()
	web := "1"        // string |  (optional)
	parentPath := "/" // string |  (optional)
	page := int32(1)  // int32 |  (optional)
	num := int32(1)   // int32 |  (optional)
	recursion := "1"  // string |  (optional)
	order := "time"   // string |  (optional)
	desc := "1"       // string |  (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfiledoclist(context.Background()).AccessToken(accessToken).ParentPath(parentPath).Recursion(recursion).Page(page).Num(num).Order(order).Desc(desc).Web(web).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfiledoclist``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "err: %v\n", r)
	}
	bodyString := string(bodyBytes)

	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfiledoclist`: %v body: %v\n", r, bodyString)
	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfiledoclist`: %v\n", resp)
}

func FileSearch() {
	accessToken := apollo.AccessToken()
	web := "1"       // string |  (optional)
	num := "2"       // string |  (optional)
	page := "1"      // string |  (optional)
	dir := "/"       // string |  (optional)
	recursion := "1" // string |  (optional)
	key := "txt"     // string |

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfilesearch(context.Background()).AccessToken(accessToken).Web(web).Num(num).Page(page).Dir(dir).Recursion(recursion).Key(key).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfilesearch``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "err: %v\n", r)
	}
	bodyString := string(bodyBytes)

	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfilesearch`: %v body: %v\n", r, bodyString)
	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfilesearch`: %v\n", resp)
}

func FileList() {
	accessToken := apollo.AccessToken()
	folder := "0"     // string |  (optional)
	web := "web"      // string |  (optional)
	start := "0"      // string |  (optional)
	limit := int32(2) // int32 |  (optional)
	dir := "/"        // string |  (optional)
	order := "time"   // string |  (optional)
	desc := int32(1)  // int32 |  (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfilelist(context.Background()).AccessToken(accessToken).Folder(folder).Web(web).Start(start).Limit(limit).Dir(dir).Order(order).Desc(desc).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfilelist``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}
	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfilelist`: %v\n", resp)
}
