cmake_minimum_required(VERSION 3.24)
project(netsim)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        include
)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        )

add_executable(netsim ${SOURCE_FILES} main.cpp)