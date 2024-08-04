cmake_minimum_required(VERSION 3.12)
project(ByteCode)

# Specifying C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# allow debugging
set(CMAKE_BUILD_TYPE Debug)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})