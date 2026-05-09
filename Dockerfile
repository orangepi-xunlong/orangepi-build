FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get -y install \
       joe \
       software-properties-common \
       gnupg \
       gnupg1 \
       gpgv1 \
       curl \
    && rm -rf /var/lib/apt/lists/*
RUN sh -c " \
    if [ $(dpkg --print-architecture) = amd64 ]; then \
        apt-get update \
        && apt-get install -y --no-install-recommends \
        lib32ncurses6 \
        lib32stdc++6 \
        lib32tinfo6 \
        libc6-i386; \
    fi"
RUN apt-get update \
    && apt-get -y upgrade \
    && apt-get install -y --no-install-recommends \
       acl \
       aptly \
       aria2 \
       bc \
       binfmt-support \
       binutils \
       bison \
       btrfs-progs \
       build-essential \
       ca-certificates \
       ccache \
       cpio \
       cryptsetup \
       cryptsetup-bin \
       debian-archive-keyring \
       debian-keyring \
       debootstrap \
       device-tree-compiler \
       dialog \
       distcc \
       dosfstools \
       dwarves \
       f2fs-tools \
       fakeroot \
       fdisk \
       flex \
       gawk \
       gcc-arm-linux-gnueabihf \
       gcc-arm-linux-gnueabi \
       gcc-arm-none-eabi \
       gcc-riscv64-linux-gnu \
       gdisk \
       git \
       imagemagick \
       jq \
       kmod \
       libbison-dev \
       libc6-amd64-cross \
       libc6-dev-armhf-cross \
       libc6-dev-riscv64-cross \
       libfdt-dev \
       libelf-dev \
       libfile-fcntllock-perl \
       libfl-dev \
       liblz4-tool \
       libncurses5-dev \
       libpython2.7-dev \
       libpython3-dev \
       libssl-dev \
       libusb-1.0-0-dev \
       linux-base \
       locales \
       lsb-release \
       lzop \
       ncurses-base \
       ncurses-term \
       nfs-kernel-server \
       ntpdate \
       openssh-client \
       p7zip-full \
       parted \
       patchutils \
       pigz \
       pixz \
       pkg-config \
       psmisc \
       pv \
       python2 \
       python3 \
       python3-dev \
       python3-distutils \
       python3-pkg-resources \
       python3-setuptools \
       rsync \
       scons \
       swig \
       sudo \
       systemd-container \
       tzdata \
       u-boot-tools \
       udev \
       unzip \
       uuid \
       uuid-dev \
       uuid-runtime \
       wget \
       whiptail \
       xfsprogs \
       xxd \
       zip \
       zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*
RUN locale-gen en_US.UTF-8

RUN git config --system --add safe.directory /root/orangepi

# Static port for NFSv3 server used for USB FEL boot
RUN sed -i 's/\(^STATDOPTS=\).*/\1"--port 32765 --outgoing-port 32766"/' /etc/default/nfs-common \
    && sed -i 's/\(^RPCMOUNTDOPTS=\).*/\1"--port 32767"/' /etc/default/nfs-kernel-server

RUN echo 'loop max_loop=128' >> /etc/modules

# 创建loop设备初始化脚本
RUN echo '#!/bin/bash' > /usr/local/bin/init-loop-devices && \
    echo 'for i in $(seq 0 7); do' >> /usr/local/bin/init-loop-devices && \
    echo '  if [ ! -e /dev/loop${i} ]; then' >> /usr/local/bin/init-loop-devices && \
    echo '    mknod /dev/loop${i} b 7 ${i} 2>/dev/null || true' >> /usr/local/bin/init-loop-devices && \
    echo '  fi' >> /usr/local/bin/init-loop-devices && \
    echo 'done' >> /usr/local/bin/init-loop-devices && \
    chmod +x /usr/local/bin/init-loop-devices

ENV LANG='en_US.UTF-8' LANGUAGE='en_US:en' LC_ALL='en_US.UTF-8' TERM=screen
WORKDIR /root/orangepi

# 复制项目文件到容器中
COPY build.sh /root/orangepi/build.sh
COPY scripts/ /root/orangepi/scripts/
COPY external/ /root/orangepi/external/

# 创建output/debug目录用于挂载
RUN mkdir -p /root/orangepi/output/debug

# 设置执行权限
RUN chmod +x /root/orangepi/build.sh

LABEL org.opencontainers.image.source="https://github.com/orangepi-xunlong/orangepi-build/blob/main/external/config/templates/Dockerfile" \
      org.opencontainers.image.authors="OrangePi Builder" \
      org.opencontainers.image.licenses="GPL-2.0"

# 启动时初始化loop设备
ENTRYPOINT ["/bin/bash", "-c", "/usr/local/bin/init-loop-devices && exec /root/orangepi/build.sh \"$@\"", "--"]