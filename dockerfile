FROM ubuntu:24.04

# Устанавливаем необходимые пакеты
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    build-essential \
    cmake \
    gcc \
    g++ \
    gdb \
    git \
    libuv1-dev \
    zlib1g-dev \
    libssl-dev \
    openssh-server \
    rsync \
    vim \
    && apt-get clean

# Клонируем и устанавливаем uWebSockets
# Клонируем репозиторий uWebSockets рекурсивно
RUN git clone --recursive https://github.com/uNetworking/uWebSockets.git /uWebSockets
# Сборка и установка uSockets (внутри uWebSockets)
RUN cd /uWebSockets/uSockets && \
    make && \
    make install

# (Опционально) можно собрать uWebSockets вручную, если тебе нужна статическая библиотека
RUN cd /uWebSockets && make

# Настраиваем SSH
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

# Открываем порт для SSH
EXPOSE 22

# Устанавливаем рабочую директорию
WORKDIR /project

# Запускаем SSH-сервер
CMD ["/usr/sbin/sshd", "-D"]