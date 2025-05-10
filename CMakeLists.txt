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
find_package(nlohmann_json REQUIRED)

# Проверка, найдены ли библиотеки
if(NOT USOCKETS_LIBRARY)
    message(FATAL_ERROR "uSockets library not found. Please ensure it is installed in /usr/local/lib")
endif()
if(NOT UV_LIBRARY)
    message(FATAL_ERROR "libuv library not found. Please ensure it is installed (e.g., sudo apt install libuv1-dev)")
endif()

# Добавление исполняемого файла
add_executable(ChatGateway main.cpp
        util/logger/AsynsLogger.cpp
        util/logger/AsynsLogger.h
        dto/ActiveUser.cpp
        dto/ActiveUser.h
        controllers/ChatController.cpp
        controllers/ChatController.h)

# Линковка библиотек
target_link_libraries(ChatGateway
    ${USOCKETS_LIBRARY}
    ${UV_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    nlohmann_json::nlohmann_json
)

target_include_directories(ChatGateway PRIVATE
        ${CMAKE_SOURCE_DIR} # <--- вот это ключевое
        /usr/local/include
        /usr/local/include/uWebSockets
)
