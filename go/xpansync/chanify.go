package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"xpansync/apollo"
)

func ChanifyText(msg string) {
	data := make(map[string]interface{})
	data["title"] = "[xpansync notification]"
	data["text"] = msg
	reqbody, _ := json.Marshal(data)

	chanify_addr := apollo.ChanifyAddr()
	fmt.Println(chanify_addr)
	resp, err := http.Post(chanify_addr, "application/json", bytes.NewReader(reqbody))
	if err != nil {
		fmt.Println(err)
	}
	respbody, _ := ioutil.ReadAll(resp.Body)
	fmt.Println(string(respbody))
}
