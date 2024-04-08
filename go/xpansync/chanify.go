package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"xpansync/sdk"
)

func ChanifyText(msg string) {
	chanify_addr := sdk.GetChanifyAddr()
	if len(chanify_addr) == 0 {
		return
	}
	data := make(map[string]interface{})
	data["title"] = "[xpansync notification]"
	data["text"] = msg
	reqbody, _ := json.Marshal(data)

	resp, err := http.Post(chanify_addr, "application/json", bytes.NewReader(reqbody))
	if err != nil {
		fmt.Println(err)
	}
	respbody, _ := io.ReadAll(resp.Body)
	fmt.Println(string(respbody))
}
