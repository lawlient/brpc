# 基于 Prometheus + Grafana + Docker 实现机器资源监控


## 部署

通过docker-compose.yml配置文件部署3个容器：

 - node-exporter
 - prometheus
 - alertmanager

`docker-compose up -d`启动容器。

### 采集任务

1. prometheus服务配置变量config.file指定Prometheus服务加载配置文件的位置。
2. 通过`scrape_configs`对采集任务进行配置，实例如下：
```yml
scrape_configs:
  - job_name: 'brpc'
    metrics_path: /brpc_metrics
    static_configs:
      - targets: ['localhost:8080']
```

- `job_name`： 表示本采集任务的名称
- `static_configs`： 表示采集地址的信息，多个目标地址通过逗号分隔
- `metrics_path`: metrics采集路径，默认是`/metrics`

### 数据上报

通过`remote_write`配置将本地采集metrics数据推送至grafana云端存储，免费用户资源优先，但个人使用基本可以满足。

需要注册grafana账户，并配置好用户名和api的密码。


## Grafana 看板配置

通过[官方地址](https://grafana.com/grafana/dashboards/)下载8919模板JSON配置文件。

将模板导入到自己的账号中。



## 待解决

目前通过增加Prometheus数据源，以Pull的方式获取数据，由于机器对外开放通过内网穿透实现。

期望通过容器内部主动Push数据至官方数据库。

- [x] Prometheus Remote Write 配置问题，HTTP响应403错误码，还是鉴权问题。

    url和username问题，没能第一时间阅读正确的官方文档。

    国内垃圾文档太多，缺乏版权保护，各种复制粘贴，作者自己都没搞明白。

