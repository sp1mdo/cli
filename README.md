# CLI - Command Line Interface Library  

**CLI** is a lightweight and efficient command-line interface library designed to provide an intuitive and user-friendly experience, similar to MikroTik or Cisco CLI. It enables developers to build powerful command-driven applications with essential features like:  

- **Command Autocompletion**: Use the `Tab` key to auto-complete commands and parameters.  
- **Command History Navigation**: Easily navigate through previous commands using the `Up` and `Down` arrow keys.  
- **Hierarchical Command Structure**: Supports structured commands with subcommands for better organization.  
- **Custom Command Registration**: Easily define and register custom commands.  
- **Built-in Help System**: Provides detailed help messages for commands.  
- **Interactive and Scripted Modes**: Can be used interactively or execute command scripts.  

## Cross-Platform Compatibility  

This library is developed in **C++**, making it highly portable to various platforms. It can be compiled and run on:  

- **Linux**
- **macOS**  
- **QNX**  
- **Windows**  
- **Raspberry Pi Pico**  

Additionally, as it is written in standard **C++**, it can be easily ported to other platforms that support C++.  

This library simplifies CLI development by handling user input, parsing commands, and providing essential usability enhancements.


## Build and Installation  

### Prerequisites  

Ensure you have the following installed:  

- **CMake (≥3.10 recommended)** – Build system generator  
- **C++ Compiler** – GCC, Clang, or MSVC with C++17 support  
- **Make or Ninja** – Build system backend (optional but recommended)  

### Building the CLI Library  

To build the project, follow these steps:  

```sh
# Create and enter the build directory
mkdir build
cd build

# Generate build files with CMake
cmake .. 

# Compile the source code
make 
```
## How to run it
```sh
  ./cli cities.txt
```
where cities.txt is just a file with set of lines made with below pattern : [continent] [country] [city]

So similarily using Prompt::insertMapElement() you can easiy create your own menu structure, where each command that contains a set of words, has corresponding function callback attached to it.


