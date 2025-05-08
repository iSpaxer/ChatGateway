# Используем Ubuntu 24.04 как базовый образ
FROM ubuntu:24.04

# Устанавливаем переменные окружения для избежания интерактивных запросов
ENV DEBIAN_FRONTEND=noninteractive

# Установка зависимостей для компиляции и SSH
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    git \
    libuv1-dev \
    libssl-dev \
    zlib1g-dev \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

# Настройка SSH
RUN mkdir /var/run/sshd \
    && echo 'root:password' | chpasswd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# Клонирование и установка uWebSockets
RUN git clone --recurse-submodules https://github.com/uNetworking/uWebSockets.git /uWebSockets \
    && cd /uWebSockets \
    && make WITH_OPENSSL=1 \
    && make install \
    && cp uSockets/uSockets.a /usr/local/lib/libuSockets.a \
    && mkdir -p /usr/local/include/uWebSockets \
    && cp -r src/* /usr/local/include/uWebSockets/ \
    && cp uSockets/src/libusockets.h /usr/local/include/ \
    && cp -r uSockets/src/*.h /usr/local/include/ \
    && ldconfig

# Создание рабочей директории
WORKDIR /app

# Копирование исходного кода проекта
COPY . /app

# Установка прав доступа для файлов
RUN chmod -R 755 /app

# Открытие портов: 22 для SSH, 9001 для WebSocket
EXPOSE 22 9001

# Команда для запуска SSH и сохранения контейнера активным
CMD ["/usr/sbin/sshd", "-D"]