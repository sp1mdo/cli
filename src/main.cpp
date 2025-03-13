#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <set>
#include <cstring>
#include <map>

#include "Prompt.hpp"

void callback(int id, const std::string &str)
{
    std::cout << "Received : id=" << id << " arg=[" << str << "]" << std::endl;
}

void special_function(int key)
{
    std::cout << "Pressed F" << key+1 << std::endl;
}

using namespace cli;

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
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }
    int fun_id = 0;
    std::string line;
    while (std::getline(file, line))
    {
        std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c)
                       { return std::tolower(c); });
        my_prompt.insertMenuItem(line, std::bind(callback, fun_id++, std::placeholders::_1));
    }

    my_prompt.Run();

    return 0;
}