package main

import (
	"xpansync/sdk"
)

type precreateReturnType struct {
	Path       string        `json:"path"`
	Uploadid   string        `json:"uploadid"`
	ReturnType int           `json:"return_type"`
	BlockList  []interface{} `json:"block_list"`
	Errno      int           `json:"errno"`
	RequestID  int64         `json:"request_id"`
}

type authReturnType struct {
	ExpiresIn     int    `json:"expires_in"`
	RefreshToken  string `json:"refresh_token"`
	AccessToken   string `json:"access_token"`
	SessionSecret string `json:"session_secret"`
	SessionKey    string `json:"session_key"`
	Scope         string `json:"scope"`
}

type deviceReturnType struct {
	DeviceCode      string `json:"device_code"`
	UserCode        string `json:"user_code"`
	VerificationURL string `json:"verification_url"`
	QrcodeURL       string `json:"qrcode_url"`
	ExpiresIn       int    `json:"expires_in"`
	Interval        int    `json:"interval"`
}

func main() {
	sdk, err := sdk.NewSdk(".appinfo.json")
	if err != nil {
		return
	}

	// 用户信息
	// info, err := sdk.Userinfo()
	// if err != nil {
	// 	fmt.Fprintf(os.Stderr, "%v\n", err)
	// 	return
	// }
	// fmt.Println(info.GetBaiduName())

	// 磁盘配额
	// quota, err := sdk.Quota()
	// if err != nil {
	// 	fmt.Fprintf(os.Stderr, "%v\n", err)
	// 	return
	// }
	// fmt.Fprintf(os.Stdout, "used: %s\ttotal:%s\n", util.Byte2IEC(quota.GetUsed()), util.Byte2IEC(quota.GetTotal()))

	// 文件列表
	// sdk.FileImageList()
	// sdk.FileDocList()
	// sdk.FileSearch()
	// sdk.FileList()

	sdk.FileUpload("./demo.txt", "/jarvis/demo.txt")
}
