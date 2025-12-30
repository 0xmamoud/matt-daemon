FROM debian:bullseye

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    gdb \
    valgrind \
    net-tools \
    netcat \
    vim \
    curl \
    procps \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /matt_daemon
CMD ["/bin/bash"]
