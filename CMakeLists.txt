cmake_minimum_required(VERSION 3.15)
project(en_utils C)

set(CMAKE_C_STANDARD 11)


add_subdirectory(lib/mbedtls)
include_directories(lib/exposure-notification/include)
include_directories(lib/mbedtls/include)

add_executable(en_utils main.c lib/exposure-notification/src/exposure-notification.c)
target_link_libraries(en_utils mbedtls)