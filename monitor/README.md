# 基于 Prometheus + Grafana + Docker 实现机器资源监控


## 部署

通过docker-compose.yml配置文件部署3个容器：

 - node-exporter
 - prometheus
 - alertmanager

`docker-compose up -d`启动容器。


## Grafana 看板配置

通过[官方地址](https://grafana.com/grafana/dashboards/)下载8919模板JSON配置文件。

将模板导入到自己的账号中。



## 待解决

目前通过增加Prometheus数据源，以Pull的方式获取数据，由于机器对外开放通过内网穿透实现。

期望通过容器内部主动Push数据至官方数据库。

- [x] Prometheus Remote Write 配置问题，HTTP响应403错误码，还是鉴权问题。

    url和username问题，没能第一时间阅读正确的官方文档。

    国内垃圾文档太多，缺乏版权保护，各种复制粘贴，作者自己都没搞明白。

