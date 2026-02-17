// SPDX-License-Identifier: MIT
// Copyright (c) 2024 sp1mdo

#include <iostream>
#include <fstream>

#include "cli.hpp"

using namespace cli;

void callback(int id, const std::string &str)
{
    std::cout << "Received : id=" << id << " arg=[" << str << "]" << std::endl;
}

void special_function(int key)
{
    std::cout << "Pressed F" << key+1 << std::endl;
}

int main(int argc, char **argv)
{
    Prompt my_prompt("WORLD");

    for(int i = static_cast<int>(FnKey::F1) ; i < static_cast<int>(FnKey::LAST_ITEM) ; i++)
    {
        my_prompt.attachFnKeyCallback(static_cast<FnKey>(i), std::bind(special_function, i));
    }

    std::ifstream file(argv[1]);

    // Check if the file was successfully opened
    if (!file.is_open())
    {
        std::cout << "Failed to open the file.\n";
        std::abort();
    }
    int fun_id = 0;
    std::string line;
    while (std::getline(file, line))
    {
        std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c)
                       { return std::tolower(c); });
        my_prompt.insertMenuItem(std::move(line), std::bind(callback, fun_id++, std::placeholders::_1));
    }

    my_prompt.Run();

    return 0;
}