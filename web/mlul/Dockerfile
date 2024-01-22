FROM node:18-alpine AS ui-builder
WORKDIR /app
COPY . . 
RUN cd ui/mlul && npm install && npm run build && cd ../..


FROM golang:alpine AS server-builder
WORKDIR /app
COPY . .
RUN go env -w GOPROXY=https://goproxy.cn
RUN cd /app/server && ls -la && go mod tidy && go build


FROM alpine:latest
ENV TZ="Asia/Shanghai"
RUN apk --no-cache --no-progress add \
    #ca-certificates \
    tzdata && \
    cp "/usr/share/zoneinfo/$TZ" /etc/localtime && \
    echo "$TZ" >  /etc/timezone
WORKDIR /app
COPY --from=server-builder /app/server/mlul /app/mlul
COPY --from=ui-builder /app/ui/mlul/dist/ /app/public

VOLUME ["/app/data"]
EXPOSE 12827