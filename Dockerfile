# === Этап сборки ===
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    git \
    libuv1-dev \
    libssl-dev \
    zlib1g-dev \
    librdkafka-dev \
 && rm -rf /var/lib/apt/lists/*

# Установка uWebSockets
RUN git clone --recurse-submodules https://github.com/uNetworking/uWebSockets.git /uWebSockets \
 && cd /uWebSockets \
 && make WITH_OPENSSL=1 \
 && make install \
 && cp uSockets/uSockets.a /usr/local/lib/libuSockets.a \
 && mkdir -p /usr/local/include/uWebSockets \
 && cp -r src/* /usr/local/include/uWebSockets/ \
 && cp uSockets/src/libusockets.h /usr/local/include/ \
 && cp -r uSockets/src/*.h /usr/local/include/ \
 && ldconfig \
 && rm -rf /uWebSockets

WORKDIR /app
COPY . /app

# Сборка проекта
RUN cmake -S /app -B /app/build \
 && cmake --build /app/build --target ChatGateway -j$(nproc) \
 && strip /app/build/ChatGateway

# === Финальный образ ===
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libuv1 \
    libssl3 \
    zlib1g \
    librdkafka1 \
    librdkafka++1 \
 && rm -rf /var/lib/apt/lists/*

# Копируем
COPY --from=builder /app/build/ChatGateway  /usr/local/bin/ChatGateway
COPY --from=builder /app/*.env /app/

# Задаём точку входа
ENTRYPOINT ["/usr/local/bin/ChatGateway", "prod"]
