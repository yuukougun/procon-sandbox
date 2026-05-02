# CUDA 版も汎用 Ubuntu 版も切り替えられるように ARG 化。
ARG BASE_IMAGE=ubuntu:22.04
FROM ${BASE_IMAGE}

# apt の対話プロンプトを抑止してビルドを安定化。
ENV DEBIAN_FRONTEND=noninteractive

# APT パッケージ定義をイメージへコピー。
COPY .devcontainer/apt-packages.txt /tmp/apt-packages.txt

# 開発に必要な基本ツールを導入。
# c++、python、Node.js、rustを導入
RUN apt-get update \
    && apt-get install -y --no-install-recommends $(grep -Ev '^\s*(#|$)' /tmp/apt-packages.txt) \
    && curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get install -y --no-install-recommends nodejs \
    && locale-gen ja_JP.UTF-8 \
    && rm -f /tmp/apt-packages.txt \
    && rm -rf /var/lib/apt/lists/*

# ロケールを日本語 UTF-8 に固定（文字化け防止）。
ENV LANG=ja_JP.UTF-8
ENV LC_ALL=ja_JP.UTF-8

# ホスト UID/GID と合わせやすいようユーザー情報を ARG で受け取る。
ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=1000

# 開発用ユーザーを作成し、sudo 権限を準備。
RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && usermod -aG sudo ${USERNAME} \
    && mkdir -p /etc/sudoers.d \
    && echo "%sudo ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/00-sudo-nopasswd \
    && chmod 440 /etc/sudoers.d/00-sudo-nopasswd

# Rust ツールチェーンの保存先を明示し、PATH に追加。
ENV CARGO_HOME=/home/${USERNAME}/.cargo
ENV RUSTUP_HOME=/home/${USERNAME}/.rustup
ENV PATH=${CARGO_HOME}/bin:${PATH}

# Rust ツール設定をテキストファイルから読み込めるようにコピー。
COPY .devcontainer/rustup-components.txt /tmp/rustup-components.txt
COPY .devcontainer/rustup-targets.txt /tmp/rustup-targets.txt
COPY .devcontainer/cargo-tools.txt /tmp/cargo-tools.txt

# Rustup をユーザー権限でインストールし、コンポーネント/ターゲット/ツールを定義ファイルから導入。
RUN su ${USERNAME} -c "curl -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable" \
    && while IFS= read -r component; do \
        if [ -n "$component" ] && [ "${component#\#}" = "$component" ]; then \
            su ${USERNAME} -c "${CARGO_HOME}/bin/rustup component add $component"; \
        fi; \
    done < /tmp/rustup-components.txt \
    && while IFS= read -r target; do \
        if [ -n "$target" ] && [ "${target#\#}" = "$target" ]; then \
            su ${USERNAME} -c "${CARGO_HOME}/bin/rustup target add $target"; \
        fi; \
    done < /tmp/rustup-targets.txt \
    && while IFS= read -r tool; do \
        if [ -n "$tool" ] && [ "${tool#\#}" = "$tool" ]; then \
            su ${USERNAME} -c "${CARGO_HOME}/bin/cargo install $tool"; \
        fi; \
    done < /tmp/cargo-tools.txt \
    && rm -f /tmp/rustup-components.txt /tmp/rustup-targets.txt /tmp/cargo-tools.txt \
    && chown -R ${USERNAME}:${USERNAME} ${CARGO_HOME} ${RUSTUP_HOME}

# Python パッケージ定義をイメージへコピー。
COPY .devcontainer/python-requirements.txt /tmp/python-requirements.txt

# python-requirements.txt は root がコピーするため、通常ユーザーが削除できるよう所有権を変更する
RUN chown ${USERNAME}:${USERNAME} /tmp/python-requirements.txt

# 以降の作業は通常ユーザーで実行。
USER ${USERNAME}

# 仮想環境を作成し、python-requirements.txt から Python パッケージを導入。
RUN python -m venv /home/${USERNAME}/.venv \
    && /home/${USERNAME}/.venv/bin/pip install --no-cache-dir --upgrade pip \
    && /home/${USERNAME}/.venv/bin/pip install --no-cache-dir -r /tmp/python-requirements.txt \
    && rm -f /tmp/python-requirements.txt

# 仮想環境を優先し、必要に応じて .local/bin も利用可能にする。
ENV PATH=/home/${USERNAME}/.venv/bin:/home/${USERNAME}/.local/bin:${PATH}

WORKDIR /workspaces
SHELL ["/bin/bash", "-c"]
CMD ["bash"]
