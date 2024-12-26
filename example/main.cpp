#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <fstream>
#include <set>
#include <cstring>
#include <map>

#include "../lib/inc/Prompt.hpp"

#define STR_TO_LOWER(str)                     \
    do                                        \
    {                                         \
        for (int i = 0; i < strlen(str); i++) \
            str[i] = tolower(str[i]);         \
    } while (0);

#define GREEN_COLOR "\033[32m"
#define DEFAULT_COLOR "\033[0m"

const char *left_key = "\x1b\x5b\x44";
const char *right_key = "\x1b\x5b\x43";


std::map<std::string, std::function<void()>> MyMenu;

void handle_special_chars(size_t &index, std::string &input)
{
    if (input == std::string(left_key))
    {
        for (size_t i = 0; i < sizeof(left_key); i++)
            input.pop_back();

        if (index > 0)
        {
            index--;
            printf("\b");
        }

        return;
    }
    if (input == std::string(right_key))
    {
        for (size_t i = 0; i < sizeof(right_key); i++)
            ;
        input.pop_back();
        if (index < input.size())
            index++;
        // clear_line();
        return;
    }
}

void callback(int id, const std::string &str)
{
    printf("Received : id=%d arg=[%s] \n ", id, str.c_str());
}

int main(int argc, char **argv)
{
    Prompt my_prompt("AHU_2040");

    std::ifstream file(argv[1]);
    
    // Check if the file was successfully opened
    /*
    if (!file.is_open())
    {
        fprintf(stderr, "Failed to open the file.");
        return 1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::transform(line.begin(), line.end(), line.begin(),  [](unsigned char c){ return std::tolower(c); });
        my_prompt.insertMapElement(line, callback);
    }
    */
   int fun_id = 0;
   my_prompt.insertMapElement("europa polska szczecin", std::bind(callback, fun_id++ , std::placeholders::_1 ));
   my_prompt.insertMapElement("europa polska wroclaw",  std::bind(callback, fun_id++ , std::placeholders::_1 ));
   my_prompt.insertMapElement("europa polska poznan",   std::bind(callback, fun_id++ , std::placeholders::_1 ));
   my_prompt.insertMapElement("afryka maroko tanger",   std::bind(callback, fun_id++ , std::placeholders::_1 ));
   my_prompt.insertMapElement("azja gruzja tbilisi",    std::bind(callback, fun_id++ , std::placeholders::_1 ));
   my_prompt.insertMapElement("azja rosja moskwa",      std::bind(callback, fun_id++ , std::placeholders::_1 ));

    my_prompt.run();

    return 0;
}