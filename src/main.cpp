#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <fstream>
#include <set>
#include <cstring>
#include <map>

#include "Prompt.hpp"

void callback(int id, const std::string &str)
{
    printf("Received : id=%d arg=[%s] \n ", id, str.c_str());
}

int main(int argc, char **argv)
{
    Prompt my_prompt("WORLD");

    std::ifstream file(argv[1]);

    // Check if the file was successfully opened
    if (!file.is_open())
    {
        fprintf(stderr, "Failed to open the file.\n");
        return 1;
    }
 int fun_id = 0;
    std::string line;
    while (std::getline(file, line))
    {
        std::transform(line.begin(), line.end(), line.begin(),  [](unsigned char c){ return std::tolower(c); });
        my_prompt.insertMapElement(std::move(line), std::bind(callback, fun_id++, std::placeholders::_1));
    }



    my_prompt.run();

    return 0;
}