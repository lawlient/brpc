#!/bin/bash


# https://github.com/outline/outline/issues/1881
# https://docs.getoutline.com/s/hosting/doc/local-development-5hEhFRXow7#h-authentication
docker exec -it outline node build/server/scripts/seed.js jiuanx2011@163.com
