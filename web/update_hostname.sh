#/usr/bin/bash

[[ $# < 1 ]] && echo "Usage: xxx newhostname" && exit;

keyword="cpolar.cn"

old=`grep $keyword -rn | grep server | grep -Ev "grep|keyword" | awk '{print $3}' | awk -F ';' '{print $1}'`

grep $keyword -rnl | grep -Ev `basename $0` | xargs sed -i "s/$old/$1/g"

grep $keyword -rn | grep -Ev "grep|keyword"

# send email to me
jarvis -m "$1"

# restart web server related to hostname

# 1. restart gateway
docker-compose -f ./nginx/docker-compose.yml up -d --build

# docker cp ./nginx/conf/default.conf gateway:/etc/nginx/conf.d/
# docker cp ./nginx/conf/index.html gateway:/etc/nginx/conf.d/
# docker restart gateway

# 2. restart jarvis_ui
docker-compose -f ./amis/docker-compose.yml up -d --build

# docker cp ./amis/index.html jarvis_ui:/amis/packages/amis/sdk/
# docker cp ./amis/jarvis.html jarvis_ui:/amis/packages/amis/sdk/
# docker restart jarvis_ui



