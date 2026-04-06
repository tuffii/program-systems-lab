FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386

RUN apt update && apt install -y \
    gcc-multilib \
    libc6-dev-i386 \
    libncurses-dev:i386 \
    libtinfo6:i386 \
    build-essential \
    make \
    file \
    && apt clean

WORKDIR /lab

CMD ["/bin/bash"]