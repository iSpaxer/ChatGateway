cmake_minimum_required(VERSION 3.10)
project(ChatGateway)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавление путей для заголовков
include_directories(/usr/local/include /usr/local/include/uWebSockets)
link_directories(/usr/local/lib)

# Поиск библиотек
find_library(USOCKETS_LIBRARY NAMES uSockets)
find_library(UV_LIBRARY NAMES uv)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Проверка, найдены ли библиотеки
if(NOT USOCKETS_LIBRARY)
    message(FATAL_ERROR "uSockets library not found. Please ensure it is installed in /usr/local/lib")
endif()
if(NOT UV_LIBRARY)
    message(FATAL_ERROR "libuv library not found. Please ensure it is installed (e.g., sudo apt install libuv1-dev)")
endif()

# Добавление исполняемого файла
add_executable(test_websocket main.cpp)

# Линковка библиотек
target_link_libraries(test_websocket
    ${USOCKETS_LIBRARY}
    ${UV_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)