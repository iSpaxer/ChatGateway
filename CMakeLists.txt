cmake_minimum_required(VERSION 3.10)
project(ChatGateway)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути для поиска заголовков
include_directories(/usr/local/include)

# Создаём исполняемый файл
add_executable(${PROJECT_NAME} main.cpp)

# Линкуем библиотеки (предполагается, что libuWS.so лежит в /usr/local/lib)
link_directories(/usr/local/lib)
target_link_libraries(${PROJECT_NAME} uWS uv z ssl crypto)
