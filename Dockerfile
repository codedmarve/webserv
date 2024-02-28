FROM ubuntu:latest

# Set noninteractive mode for apt-get
ARG DEBIAN_FRONTEND=noninteractive

# Install Zsh and other required packages
RUN apt-get update && \
    apt-get install -y zsh \
    build-essential \
    valgrind \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxrender-dev \
    libbsd-dev \
    libffi-dev \
    liblzma-dev \
    libbz2-dev \
    clang \
    clang-format \
    cmake \
    git \
    curl \
    libcurl4-openssl-dev  # Add libcurl4-openssl-dev here

# Set the default shell to Zsh
SHELL ["/bin/zsh", "-c"]

# Install Oh My Zsh and plugins
RUN [ ! -d "/root/.oh-my-zsh" ] && sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" || true && \
    [ ! -d "/root/.oh-my-zsh/custom/plugins/zsh-syntax-highlighting" ] && git clone https://github.com/zsh-users/zsh-syntax-highlighting.git /root/.oh-my-zsh/custom/plugins/zsh-syntax-highlighting || true && \
    [ ! -d "/root/.oh-my-zsh/custom/plugins/zsh-autosuggestions" ] && git clone https://github.com/zsh-users/zsh-autosuggestions.git /root/.oh-my-zsh/custom/plugins/zsh-autosuggestions || true && \
    sed -i 's/plugins=(git)/plugins=(git z common-aliases node npm rand-quote sudo colored-man-pages colorize cp zsh-syntax-highlighting zsh-autosuggestions copyfile history dirhistory)/' /root/.zshrc

# Install Google Test
RUN git clone https://github.com/google/googletest.git /usr/src/gtest && \
    cmake -S /usr/src/gtest -B /usr/src/gtest/build && \
    cmake --build /usr/src/gtest/build --target install

# Set symlinks for clang and clang++
RUN ln -sf /usr/bin/clang /usr/bin/cc && \
    ln -sf /usr/bin/clang++ /usr/bin/c++

# Set the working directory
WORKDIR /app
