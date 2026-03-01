# ベースOS
FROM ubuntu:22.04

# aptを非対話化
ENV DEBIAN_FRONTEND=noninteractive

# パッケージリスト更新
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        g++ \
        make \
        cmake \
        git \
        sudo \
        python3 \
        python3-pip \
        python3-venv \
        doxygen \
        graphviz \
        ca-certificates \
        pkg-config \
    && rm -rf /var/lib/apt/lists/*

ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=1000
RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && usermod -aG sudo ${USERNAME} \
    && mkdir -p /etc/sudoers.d \
    && echo "%sudo ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/00-sudo-nopasswd \
    && chmod 440 /etc/sudoers.d/00-sudo-nopasswd

USER ${USERNAME}
WORKDIR /workspaces/procon-sandbox

SHELL ["/bin/bash", "-c"]

CMD ["bash"]
