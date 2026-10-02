cmake_minimum_required(VERSION 3.20)
project(parallel)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

add_library(stb_headers INTERFACE)
target_include_directories(stb_headers INTERFACE ${stb_SOURCE_DIR})

add_executable(parallel 
    # lab1.cpp
    lab2.cpp
)

target_link_libraries(parallel PRIVATE stb_headers)