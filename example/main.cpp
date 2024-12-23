#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
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

void callback(std::string str)
{
    printf("Received : %s \n ", str.c_str());
}

int main(int argc, char **argv)
{
    Prompt my_prompt;
    my_prompt.insertMapElement("europa polska szczecin centrum", callback);
    my_prompt.insertMapElement("europa polska szczecin turzyn", callback);
    my_prompt.insertMapElement("europa polska szczecin pogodno", callback);
    my_prompt.insertMapElement("europa polska szczecin niebuszewo", callback);
    my_prompt.insertMapElement("europa polska szczecin warszewo", callback);
    my_prompt.insertMapElement("europa polska warszawa", callback);
    my_prompt.insertMapElement("europa polska bialystok", callback);
    my_prompt.insertMapElement("europa polska zakopane", callback);
    my_prompt.insertMapElement("europa niemcy berlin", callback);
    my_prompt.insertMapElement("europa niemcy frankfurt", callback);
    my_prompt.insertMapElement("europa francja paryz", callback);
    my_prompt.insertMapElement("europa szwecja sztokholm", callback);
    my_prompt.insertMapElement("europa hiszpania barcelona", callback);
    my_prompt.insertMapElement("europa norwegia", callback);
    my_prompt.insertMapElement("afryka kongo", callback);
    my_prompt.insertMapElement("azja rosja moskwa", callback);
    my_prompt.insertMapElement("azja rosja soczi", callback);
    my_prompt.insertMapElement("azja rosja irkutsk", callback);
    my_prompt.insertMapElement("azja rosja kursk", callback);
    my_prompt.insertMapElement("europa czechy praga", callback);
    my_prompt.insertMapElement("azja gruzja tbilisi", callback);
    my_prompt.insertMapElement("ameryka usa waszyngton", callback);
    my_prompt.insertMapElement("ameryka usa chicago", callback);


    
    

    //my_prompt.init(&main);
    my_prompt.setNonCanonicalMode();
    my_prompt.run();

    return 0;
}