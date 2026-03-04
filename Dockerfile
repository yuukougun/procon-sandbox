# CUDA 版も汎用 Ubuntu 版も切り替えられるように ARG 化。
ARG BASE_IMAGE=ubuntu:22.04
FROM ${BASE_IMAGE}

# apt の対話プロンプトを抑止してビルドを安定化。
ENV DEBIAN_FRONTEND=noninteractive

# 開発に必要な基本ツール、GUI 実行に必要なランタイムを導入。
# c++、python、Node.js、rustを導入
# visualizer をコンテナ内で表示するために X11/Wayland 関連ライブラリを含める。
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

# ロケールを日本語 UTF-8 に固定（文字化け防止）。
ENV LANG=ja_JP.UTF-8
ENV LC_ALL=ja_JP.UTF-8

# ホスト UID/GID と合わせやすいようユーザー情報を ARG で受け取る。
ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=1000

# 開発用ユーザーを作成し、sudo 権限と XDG ランタイムディレクトリを準備。
RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && usermod -aG sudo ${USERNAME} \
    && mkdir -p /etc/sudoers.d \
    && echo "%sudo ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/00-sudo-nopasswd \
    && chmod 440 /etc/sudoers.d/00-sudo-nopasswd \
    && mkdir -p /tmp/xdg-runtime \
    && chown ${USERNAME}:${USERNAME} /tmp/xdg-runtime \
    && chmod 700 /tmp/xdg-runtime

# Rust ツールチェーンの保存先を明示し、PATH に追加。
ENV CARGO_HOME=/home/${USERNAME}/.cargo
ENV RUSTUP_HOME=/home/${USERNAME}/.rustup
ENV PATH=${CARGO_HOME}/bin:${PATH}

# Rustup をユーザー権限でインストールし、フォーマット/静的解析ツールを有効化。
RUN su ${USERNAME} -c "curl -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable" \
    && su ${USERNAME} -c "${CARGO_HOME}/bin/rustup component add rustfmt clippy" \
    && chown -R ${USERNAME}:${USERNAME} ${CARGO_HOME} ${RUSTUP_HOME}

# 以降の作業は通常ユーザーで実行。
USER ${USERNAME}
WORKDIR /workspaces/procon-sandbox
SHELL ["/bin/bash", "-c"]
CMD ["bash"]
