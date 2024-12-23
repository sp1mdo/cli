# Project name and language
project(CLIlib LANGUAGES CXX)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
    "src/Prompt.cpp"
)
if (WIN32)
    message(WARNING "Building on Windows.")
    add_definitions(-DWIN32)
    # Add Windows-specific settings here
elseif (UNIX)
    message(WARNING "Building on Linux/Unix.")
    add_definitions(-DUNIX)
    # Add Linux-specific settings here
else()
    message(WARNING "Unknown platform.")
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})


target_include_directories(${PROJECT_NAME} PRIVATE
    "inc"
)

target_compile_options(CLIlib PRIVATE
    -Wall
    -Werror
)
