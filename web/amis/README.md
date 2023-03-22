# [amis](https://github.com/baidu/amis)

前端低代码框架，通过 JSON 配置就能生成各种后台页面，极大减少开发成本，甚至可以不需要了解前端。

## 使用

官方文档介绍了基本使用方法，本文基于官方文档的基础上，通过docker实现安装使用。

### 依赖

本地完成docker安装，可以参考[官方文档](https://docs.docker.com/get-docker/)完成安装。

### 构建镜像

与Dockerfile所在路径下执行：

```bash
docker build . -f Dockerfile -t amis
```

### 构建容器

待镜像构建完成后，执行如下命令启动hello服务

```bash
docker run -itd --name jarvis_ui -p 826:3000 amis
```

执行完成后可以通过 `docker ps -a | grep amis`查看容器状态。

### 测试

打开浏览器，地址栏输入: `http://localhost:3000/hello`，正常情况下可以看到如下页面。

![示例页面](data/hello.png)
