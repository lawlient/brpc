FROM nginx:latest

RUN apt update && \
    apt install -y lsof dnsutils procps iputils-ping net-tools

COPY ./conf/* /etc/nginx/conf.d/


