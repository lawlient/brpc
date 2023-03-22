FROM ubuntu:latest

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    ca-certificates


RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    build-essential cmake \
    && apt-get clean -y

COPY ./ /code/log

WORKDIR /code/log

RUN mkdir build; cd build && rm * -rf ; cmake .. && make -j$(nproc) && make install && \
    cd ../ && rm build -rf

CMD log_agent
