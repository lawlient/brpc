package docker

import (
	"context"
	"fmt"
	"log"
	"os/exec"

	"github.com/docker/docker/api/types"
	"github.com/docker/docker/api/types/container"
	"github.com/docker/docker/client"
)

type DockerSDK struct {
	docker *client.Client
	conid  string
}

func (sdk *DockerSDK) Run(config *container.Config, host *container.HostConfig) error {
	docker, err := client.NewClientWithOpts()
	if err != nil {
		fmt.Println(err)
		return err
	}
	sdk.docker = docker

	cont, err := docker.ContainerCreate(context.Background(), config, host, nil, nil, "")
	if err != nil {
		fmt.Println(err)
		return err
	}
	// 启动容器
	if err := docker.ContainerStart(context.Background(), cont.ID, types.ContainerStartOptions{}); err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Container %s is started\n", cont.ID)
	sdk.conid = cont.ID
	return nil
}

func (sdk *DockerSDK) Exec2(cmd string) error {
	dcmd := "docker exec " + sdk.conid + " sh -c \"" + cmd + "\""
	fmt.Printf("/$ CMD: %s\n", dcmd)
	stdout, err := exec.Command("/bin/sh", "-c", dcmd).Output()
	if err != nil {
		fmt.Println(err)
		return err
	}

	fmt.Println("/$ ", string(stdout))
	return nil
}

func (sdk *DockerSDK) Remove() {
	// 停止容器
	if err := sdk.docker.ContainerStop(context.Background(), sdk.conid, container.StopOptions{}); err != nil {
		fmt.Println(err)
	}
	fmt.Printf("Container %s is stoped\n", sdk.conid)

	// 删除容器
	if err := sdk.docker.ContainerRemove(context.Background(), sdk.conid, types.ContainerRemoveOptions{}); err != nil {
		fmt.Println(err)
	}
	fmt.Printf("Container %s is removed\n", sdk.conid)

	// 关闭Docker客户端连接
	defer sdk.docker.Close()
}
