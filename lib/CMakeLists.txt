# Project name and language
project(CLIlib LANGUAGES CXX)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
    "src/Menu.cpp"
    "src/MenuEntity.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})


target_include_directories(${PROJECT_NAME} PRIVATE
    "inc"
)

target_compile_options(CLIlib PRIVATE
    -Wall
    -Werror
)
