FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

ENV TZ=Asia/Tokyo
RUN apt-get update && apt-get install -y --no-install-recommends tzdata && \
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get install -y locales && \
    rm -rf /var/lib/apt/lists/* && \
    localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
ENV LANG=en_US.UTF-8

RUN apt-get update && apt-get install -y --no-install-recommends \
    acl aptly aria2 bc binfmt-support bison btrfs-progs \
    build-essential ca-certificates ccache cpio cryptsetup curl \
    debian-archive-keyring debian-keyring debootstrap device-tree-compiler \
    dialog dirmngr dosfstools dwarves f2fs-tools fakeroot flex gawk \
    gcc-arm-linux-gnueabihf gdisk git gpg imagemagick jq kmod libbison-dev \
    libc6-dev-armhf-cross libelf-dev libfdt-dev libfile-fcntllock-perl \
    libfl-dev liblz4-tool libncurses-dev libpython2.7-dev libssl-dev \
    libusb-1.0-0-dev linux-base lzop ncurses-base ncurses-term \
    nfs-kernel-server ntpdate p7zip-full parted patchutils pigz pixz \
    pkg-config pv python3-dev python3-distutils qemu-user-static rsync swig \
    systemd-container u-boot-tools udev unzip uuid-dev wget whiptail zip \
    zlib1g-dev gcc-riscv64-linux-gnu uuid-runtime fatattr git-lfs scons \
    mtools python3 python-is-python3 sudo vim nano \
    distcc lib32ncurses-dev lib32stdc++6 libc6-i386 \
    && rm -rf /var/lib/apt/lists/*

RUN git config --global --add safe.directory '*'
WORKDIR /workdir

ENTRYPOINT ["/bin/bash"]