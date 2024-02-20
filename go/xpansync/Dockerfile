FROM golang:alpine AS builder
WORKDIR /app
COPY . .
ENV GOPROXY=https://goproxy.cn
RUN go mod tidy && go build


FROM alpine:latest
WORKDIR /app
ENV TZ="Asia/Shanghai"
RUN apk --no-cache --no-progress add tzdata && \
    cp "/usr/share/zoneinfo/$TZ" /etc/localtime && \
    echo "$TZ" >  /etc/timezone
COPY --from=builder /app/xpansync /app/xpansync



