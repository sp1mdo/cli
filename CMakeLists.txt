cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(cli VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target
add_executable(cli
    src/main.cpp
    src/Prompt.cpp
)

# Define Debug and Release specific compile options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Configuring Debug Build")
    add_compile_definitions(DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Configuring Release Build")
    add_compile_definitions(RELEASE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
endif()

target_compile_options(cli PRIVATE
    -Wall
    -Werror
)
