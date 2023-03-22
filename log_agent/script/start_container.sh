#/usr/bin/bash


docker ps -a | grep log_agent && exit $?

docker run -itd --name log_agent --ipc=shareable -v /data:/data -v /etc/localtime:/etc/localtime:ro log_agent

docker ps -a | grep Up | grep log_agent || exit $?


