cmake_minimum_required(VERSION 3.10)
project(ChatGateway)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------
# Dependencies
# --------------------
find_library(USOCKETS_LIBRARY NAMES uSockets PATHS /usr/local/lib)
find_library(UV_LIBRARY NAMES uv PATHS /usr/local/lib)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Manually find librdkafka
find_library(RDKAFKA_LIBRARY NAMES rdkafka PATHS /usr/lib/x86_64-linux-gnu)
find_library(RDKAFKAXX_LIBRARY NAMES rdkafka++ PATHS /usr/lib/x86_64-linux-gnu)
find_path(RDKAFKA_INCLUDE_DIR NAMES rdkafkacpp.h PATHS /usr/include/librdkafka)

# Check if librdkafka was found
if(NOT RDKAFKA_LIBRARY OR NOT RDKAFKAXX_LIBRARY OR NOT RDKAFKA_INCLUDE_DIR)
    message(FATAL_ERROR "librdkafka not found. Please install librdkafka-dev")
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
FetchContent_Declare(
        jwt-cpp
        GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
        GIT_TAG v0.6.0
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
target_link_libraries(ChatGateway PRIVATE
        ${USOCKETS_LIBRARY}
        ${UV_LIBRARY}
        OpenSSL::SSL
        OpenSSL::Crypto
        ZLIB::ZLIB
        ${RDKAFKA_LIBRARY}
        ${RDKAFKAXX_LIBRARY}
        nlohmann_json::nlohmann_json
        jwt-cpp::jwt-cpp
        dl
)

# --------------------
# Include directories
# --------------------
target_include_directories(ChatGateway PRIVATE
        ${CMAKE_SOURCE_DIR}
        /usr/local/include  # For uWebSockets
        ${RDKAFKA_INCLUDE_DIR}  # For librdkafka
)