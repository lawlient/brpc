FROM nginx:latest

# 替换为中科大镜像源，默认debian镜像源国内访问太慢
RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list.d/debian.sources

RUN apt update && \
    apt install -y lsof dnsutils procps iputils-ping net-tools
