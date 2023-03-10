# brpc services


## Build Image

You should build all services with Docker.

```
docker build -f Dockerfile -t brpc .

docker run -itd --name brpc -v/data:/data brpc

docker exec -it brpc bash
```

I will keep updating Dockerfile for services' rely on.


## Build Service

```
cd echo_c++
mkdir build
cd build
cmake .. 
make
```


