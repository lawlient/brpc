#/usr/bin/bash


killall log_agent

./log_agent


ps aux | grep log_agent | grep -Ev "grep"
