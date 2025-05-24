cmake_minimum_required(VERSION 3.10)
project(ChatGateway)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------
# Dependencies
# --------------------
find_library(USOCKETS_LIBRARY NAMES uSockets PATHS /usr/local/lib)
find_library(UV_LIBRARY     NAMES uv       PATHS /usr/local/lib)
find_package(OpenSSL REQUIRED)
find_package(ZLIB    REQUIRED)


# Проверка зависимостей
if(NOT USOCKETS_LIBRARY)
    message(FATAL_ERROR "uSockets library not found. Please install libuSockets in /usr/local/lib")
endif()
if(NOT UV_LIBRARY)
    message(FATAL_ERROR "libuv library not found. Please install libuv1-dev")
endif()

# --------------------
# Fetch nlohmann_json
# --------------------
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# --------------------
# Fetch jwt-cpp
# --------------------
include(FetchContent)
FetchContent_Declare(
        jwt-cpp
        GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
        GIT_TAG        v0.6.0
)
FetchContent_MakeAvailable(jwt-cpp)

# --------------------
# Executable
# --------------------
add_executable(ChatGateway
        main.cpp
        util/logger/AsynsLogger.cpp
        dto/ActiveUser.cpp
        controllers/ChatController.cpp
        jwt/config/JwtConfigure.cpp
        dto/jwt/JwtToken.h
)

# --------------------
# Link libraries
# --------------------
# Note: jwt-cpp::jwt-cpp and nlohmann_json::nlohmann_json automatically bring include dirs
# No manual include_directories needed for these

target_link_libraries(ChatGateway PRIVATE
        ${USOCKETS_LIBRARY}
        ${UV_LIBRARY}
        OpenSSL::SSL
        OpenSSL::Crypto
        ZLIB::ZLIB
        nlohmann_json::nlohmann_json
        jwt-cpp::jwt-cpp
)

target_include_directories(ChatGateway PRIVATE
        ${CMAKE_SOURCE_DIR} #
)
