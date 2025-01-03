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

target_include_directories(${PROJECT_NAME} PRIVATE
    "inc"
)

target_compile_options(cli PRIVATE
    -Wall
    -Werror
)
