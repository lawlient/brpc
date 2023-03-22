# brpc services

Those services are built based on brpc repository.

I build all of it in docker container, but officail Dockerfile in brpc repository has some errors for me.

I modify [it](Dockerfile) to work successfully.

You can make it as follows.  Let's start.

## Build compiler enviorment

You should build all services with Docker. Do as follows:

```bash
# build image from dockerfile
docker build -f Dockerfile -t brpc .

# start container, and my code lies in data directory.
docker run -itd --name brpc -v/data:/data brpc

# enter container
docker exec -it brpc bash
```

Now, you can compile all services in container.


## Build Service

Eg. Compiling echo_c++ service in container.

```bash
cd echo_c++
mkdir build
cd build
cmake .. && make
```


