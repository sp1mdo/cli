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

# Include directories for header files
target_include_directories(cli PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PRIVATE
    "inc"
)

target_compile_options(cli PRIVATE
    -Wall
    -Werror
)
