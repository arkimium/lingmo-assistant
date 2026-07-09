FROM docker.m.daocloud.io/library/debian:bookworm-slim

RUN sed -i 's/deb.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list.d/debian.sources 2>/dev/null || \
    sed -i 's/deb.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list 2>/dev/null || true

RUN apt-get update || apt-get update || apt-get update

RUN apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev \
    qt6-base-private-dev \
    qt6-declarative-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qt6-svg-dev \
    qt6-l10n-tools \
    libqt6network6 \
    libqt6opengl6-dev \
    libgl1-mesa-dev \
    libvulkan-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    devscripts \
    debhelper \
    lintian \
    file \
    || apt-get install -y --no-install-recommends --fix-missing \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-declarative-dev qt6-tools-dev qt6-tools-dev-tools \
    qt6-svg-dev libqt6network6 libgl1-mesa-dev libvulkan-dev \
    libcurl4-openssl-dev libssl-dev devscripts debhelper lintian file \
    || (apt-get update && apt-get install -y devscripts debhelper lintian file)

RUN apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
