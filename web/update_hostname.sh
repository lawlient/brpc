#/usr/bin/bash

[[ $# < 1 ]] && echo "Usage: xxx newhostname" && exit;

keyword="cpolar.cn"

old=`grep $keyword -rn | grep server | grep -Ev "grep|keyword" | awk '{print $3}' | awk -F ';' '{print $1}'`

grep $keyword -rnl | grep -Ev `basename $0` | xargs sed -i "s/$old/$1/g"

grep $keyword -rn | grep -Ev "grep|keyword"


# restart web server related to hostname

# 1. restart gateway
docker-compose -f ./nginx/docker-compose.yml up -d --build

# 2. restart jarvis_ui
docker-compose -f ./amis/docker-compose.yml up -d --build




