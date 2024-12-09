cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(CLIExample
    main.cpp
)

# Check if compiling on Windows
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

target_link_libraries(CLIExample PRIVATE CLIlib)
