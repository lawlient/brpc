package sdk

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	openapiclient "xpansync/openxpanapi"

	"xpansync/xlog"
)

type FileRequest struct {
	AccessToken string `json:"access_token"`
	Web         string `json:"web"`
	ParentPath  string `json:"parent_path"`
	Recursion   string `json:"recursion"`
	Page        int32  `json:"page"`
	Num         int32  `json:"num"`
	Order       string `json:"order"`
	Desc        string `json:"desc"`
	Folder      string `json:"folder"`
	Start       string `json:"start"`
	Limit       int32  `json:"limit"`
	ShowEmpty   int32  `json:"show_empty"`
}

func NewFileRequest() *FileRequest {
	req := &FileRequest{}
	req.AccessToken = getToken()
	req.Web = "1"            // string |  (optional) 为1时返回文档预览地址lodocpreview
	req.ParentPath = "/"     // string |  (optional) 目录名称，以/开头的绝对路径, 默认为/ , 路径包含中文时需要UrlEncode编码
	req.Recursion = "1"      // string |  (optional) 是否递归，0为否，1为是. 递归是指：当目录下有文件夹，可以获取到文件夹下面的文档
	req.Page = int32(1)      // int32 |  (optional) 页码，从1开始， 如果不指定页码，则为不分页模式，返回所有的结果。如果指定page参数，则按修改时间倒序排列
	req.Num = int32(10)      // int32 |  (optional) 一页返回的文档数， 默认值为1000，建议最大值不超过1000
	req.Order = "time"       // string |  (optional) time(default), size, name
	req.Desc = "1"           // string |  (optional) 0为升序，1为降序，默认为1
	req.Folder = "0"         // string | 是否只返回文件夹，0 返回所有，1 只返回文件夹，且属性只返回path字段
	req.Start = "0"          // string | 起始位置，从0开始
	req.Limit = int32(1000)  // int32  | 查询数目，默认为1000，建议最大不超过1000
	req.ShowEmpty = int32(0) // int32 | 是否返回dir_empty属性，0 不返回，1 返回
	return req
}

func (r *FileRequest) ToJsonString() string {
	j, err := json.Marshal(r)
	if err != nil {
		return err.Error()
	}
	return string(j)
}

// 本接口用于获取用户指定目录下的图片列表
// https://pan.baidu.com/union/doc/bksg0sayv
func FileImageList(req *FileRequest) (string, error) {
	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfileimagelist(context.Background()).AccessToken(req.AccessToken).ParentPath(req.ParentPath).Recursion(req.Recursion).Page(req.Page).Num(req.Num).Order(req.Order).Desc(req.Desc).Web(req.Web).Execute()
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
	return bodyString, err
}

// 本接口用于获取用户指定目录下的文档列表
// https://pan.baidu.com/union/doc/Eksg0saqp
func FileDocList(req *FileRequest) {
	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfiledoclist(context.Background()).AccessToken(req.AccessToken).ParentPath(req.ParentPath).Recursion(req.Recursion).Page(req.Page).Num(req.Num).Order(req.Order).Desc(req.Desc).Web(req.Web).Execute()
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

// 本接口用于获取用户指定目录下，包含指定关键字的文件列表
// https://pan.baidu.com/union/doc/zksg0sb9z
func FileSearch(req *FileRequest, key string) {
	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfilesearch(context.Background()).AccessToken(req.AccessToken).Web(req.Web).Num(req.Num).Page(req.Page).Dir(req.ParentPath).Recursion(req.Recursion).Key(key).Execute()
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

// 本接口用于获取用户网盘中指定目录下的文件列表。返回的文件列表支持排序、分页等操作。
// https://pan.baidu.com/union/doc/nksg0sat9
func FileList(req *FileRequest) (string, error) {
	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileinfoApi.Xpanfilelist(context.Background()).AccessToken(req.AccessToken).Folder(req.Folder).Web(req.Web).Start(req.Start).Limit(req.Limit).Dir(req.ParentPath).Order(req.Order).Desc(req.Desc).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileinfoApi.Xpanfilelist``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}
	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfilelist`: %v\n", resp)

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "err: %v\n", r)
	}
	bodyString := string(bodyBytes)

	fmt.Fprintf(os.Stdout, "Response from `FileinfoApi.Xpanfileimagelist`: %v body: %v\n", r, bodyString)
	return bodyString, err
}

func Filelist(w http.ResponseWriter, r *http.Request) {
	req := NewFileRequest()
	req.ParentPath = r.Header.Get("dir")
	body, _ := FileList(req)
	xlog.Logger.Info("success", "request", req)
	fmt.Fprintf(w, "%s\n", body)
}
