package sdk

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"strings"

	openapiclient "xpansync/openxpanapi"
	"xpansync/util"
)

// https://pan.baidu.com/union/doc/3ksg0s9r7
type UploadRequest struct {
	Path      string // 上传后的绝对路径
	Isdir     int32  // 0：文件，1：目录
	Size      int32  // 上传文件时，表示文件的大小，单位B；上传目录时，表示目录的大小，目录的话大小默认为0
	Autoinit  int32  // 固定值：1
	BlockList string
	Rtype     int32  // 文件命名策略。 1:表示当path冲突时，进行重命名 2:表示当path冲突且block_list不同时，进行重命名 3:当云端存在同名文件时，对该文件进行覆盖
	ID        string // 上传任务id，由precreate返回
}

type precreateReturnType struct {
	Path       string        `json:"path"`
	Uploadid   string        `json:"uploadid"`
	ReturnType int           `json:"return_type"`
	BlockList  []interface{} `json:"block_list"`
	Errno      int           `json:"errno"`
	RequestID  int64         `json:"request_id"`
}

// src: 本地待上传文件
// dsc: 网盘绝对路径
func (sdk *sdk) FileUpload(src string, dsc string) {
	file, err := os.Open(src)
	if err != nil {
		fmt.Fprintf(os.Stderr, "open %s failed, err:%v", src, err)
		return
	}
	stat, err := file.Stat()
	if err != nil {
		fmt.Fprintf(os.Stderr, "stat %s failed, err:%v", src, err)
		return
	}
	var req UploadRequest
	req.Path = dsc // string
	if stat.IsDir() {
		req.Isdir = 1
		fmt.Fprintf(os.Stderr, "dir is not supported now, coming soon")
		return
	} else {
		req.Isdir = 0
		if stat.Size() > (4 << 30) {
			fmt.Fprintf(os.Stderr, "file size react limit 4GB")
			return
		}
	}
	req.Size = int32(stat.Size()) // int32
	req.Autoinit = int32(1)       // int32
	req.Rtype = int32(3)          // overwrite

	blocks := make([]string, 0)
	for {
		buf := make([]byte, 4<<20)
		n, err := file.Read(buf)
		if err != nil && err != io.EOF {
			fmt.Fprintf(os.Stderr, "read file fail,err:%v\n", err)
			return
		}
		if n == 0 {
			break
		}
		blocks = append(blocks, "\""+util.Md5String(buf)+"\"")
	}

	req.BlockList = "[" + strings.Join(blocks, ",") + "]" // string

	sdk.fileprecreate(&req)
	sdk.superfile2(&req, src)
	sdk.filecreate(&req)
}

// 创建上传任务
func (sdk *sdk) fileprecreate(req *UploadRequest) {
	accessToken := sdk.config.AccessToken // string
	path := req.Path                      // string
	isdir := req.Isdir                    // int32
	size := req.Size                      // int32
	autoinit := req.Autoinit              // int32
	blockList := req.BlockList            // string
	rtype := req.Rtype                    // int32 | rtype (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileuploadApi.Xpanfileprecreate(context.Background()).AccessToken(accessToken).Path(path).Isdir(isdir).Size(size).Autoinit(autoinit).BlockList(blockList).Rtype(rtype).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileuploadApi.Xpanfileprecreate``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}
	// response from `Xpanfileprecreate`: Fileprecreateresponse
	fmt.Fprintf(os.Stdout, "Response from `FileuploadApi.Xpanfileprecreate`: %v\n", resp)

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "err: %v\n", r)
	}

	p := &precreateReturnType{}
	if err := json.Unmarshal([]byte(string(bodyBytes)), p); err != nil {
		fmt.Fprintf(os.Stderr, "unmarshal precreate response fail, err:%v\n", err)
		return
	}
	req.ID = p.Uploadid
	fmt.Fprintf(os.Stdout, "uploadid: %s\n", req.ID)
}

// 分片上传
func (sdk *sdk) superfile2(req *UploadRequest, name string) {
	accessToken := sdk.config.AccessToken // string
	path := req.Path
	uploadid := req.ID
	type_ := "tmpfile"
	file, err := os.Open(name)
	if err != nil {
		fmt.Println(err)
	}
	defer file.Close()

	configuration := openapiclient.NewConfiguration()
	//configuration.Debug = true
	api_client := openapiclient.NewAPIClient(configuration)

	blocks := make([]string, 0)
	loop := 0
	for {
		buf := make([]byte, 4<<20)
		n, err := file.Read(buf)
		if err != nil && err != io.EOF {
			fmt.Fprintf(os.Stderr, "read file fail,err:%v\n", err)
			return
		}
		if n == 0 {
			break
		}
		resp, r, err := api_client.FileuploadApi.Pcssuperfile2(context.Background()).AccessToken(accessToken).Partseq(partseq).Path(path).Uploadid(uploadid).Type_(type_).File(file).Execute()

		if err != nil {
			fmt.Fprintf(os.Stderr, "Error when calling `FileuploadApi.Pcssuperfile2``: %v\n", err)
			fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
		}
		// response from `Pcssuperfile2`: string
		fmt.Fprintf(os.Stdout, "Response from `FileuploadApi.Pcssuperfile2`: %v\n", resp)

		bodyBytes, err := ioutil.ReadAll(r.Body)
		if err != nil {
			fmt.Fprintf(os.Stderr, "err: %v\n", r)
		}

		fmt.Println(string(bodyBytes))
	}
}

// 合并文件
func (sdk *sdk) filecreate(req *UploadRequest) {
	accessToken := sdk.config.AccessToken // string
	path := req.Path                      // string
	isdir := req.Isdir                    // int32
	size := req.Size                      // int32
	uploadid := req.ID                    // string
	blockList := req.BlockList            // string
	rtype := req.Rtype                    // int32 | rtype (optional)

	configuration := openapiclient.NewConfiguration()
	api_client := openapiclient.NewAPIClient(configuration)
	resp, r, err := api_client.FileuploadApi.Xpanfilecreate(context.Background()).AccessToken(accessToken).Path(path).Isdir(isdir).Size(size).Uploadid(uploadid).BlockList(blockList).Rtype(rtype).Execute()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error when calling `FileuploadApi.Xpanfilecreate``: %v\n", err)
		fmt.Fprintf(os.Stderr, "Full HTTP response: %v\n", r)
	}
	// response from `Xpanfilecreate`: Filecreateresponse
	fmt.Fprintf(os.Stdout, "Response from `FileuploadApi.Xpanfilecreate`: %v\n", resp)

	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "err: %v\n", r)
	}

	fmt.Println(string(bodyBytes))
}