# ベースOS（デフォルトは汎用 Ubuntu。GPU を使いたい場合は build-arg BASE_IMAGE を CUDA イメージに差し替える）
ARG BASE_IMAGE=ubuntu:22.04
FROM ${BASE_IMAGE}

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
        openssh-client \
        gh \
        locales \
        curl \
        gnupg \
        xauth \
        x11-xserver-utils \
        libx11-6 \
        libxcb1 \
        libxrandr2 \
        libxi6 \
        libxinerama1 \
        libxcursor1 \
        libxxf86vm1 \
        libxkbcommon0 \
        libxkbcommon-x11-0 \
        libwayland-client0 \
        libwayland-egl1 \
        libegl1 \
        libglx0 \
        libgl1 \
        libasound2 \
        libfontconfig1 \
        libgtk-3-0 \
    && curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get install -y --no-install-recommends nodejs \
    && locale-gen ja_JP.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

ENV LANG ja_JP.UTF-8
ENV LC_ALL ja_JP.UTF-8

ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=1000
RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && usermod -aG sudo ${USERNAME} \
    && mkdir -p /etc/sudoers.d \
    && echo "%sudo ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/00-sudo-nopasswd \
    && chmod 440 /etc/sudoers.d/00-sudo-nopasswd \
    && mkdir -p /tmp/xdg-runtime \
    && chown ${USERNAME}:${USERNAME} /tmp/xdg-runtime \
    && chmod 700 /tmp/xdg-runtime

ENV CARGO_HOME=/home/${USERNAME}/.cargo
ENV RUSTUP_HOME=/home/${USERNAME}/.rustup
ENV PATH="${CARGO_HOME}/bin:${PATH}"

RUN su ${USERNAME} -c "curl -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable" \
    && su ${USERNAME} -c "${CARGO_HOME}/bin/rustup component add rustfmt clippy" \
    && chown -R ${USERNAME}:${USERNAME} ${CARGO_HOME} ${RUSTUP_HOME}

USER ${USERNAME}
WORKDIR /workspaces/procon-sandbox

SHELL ["/bin/bash", "-c"]

CMD ["bash"]
