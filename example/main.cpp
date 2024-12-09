#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <set>
#include <cstring>


#include "../lib/inc/Menu.hpp"
#include "../lib/inc/MenuEntity.hpp"
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

void callback(std::vector<int> argv)
{
    printf("Received : ");
    for(auto & arg : argv)
    {
        printf("%d, ",arg);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    
    Prompt my_prompt;
    Menu main("main");

    Menu ssaki("ssaki");
    Menu gady("gady");
    Menu plazy("plazy");
    Menu ptaki("ptaki");

    Menu koty("koty");

    main.emplace_back(MenuEntity("ssaki"), ssaki);
    main.emplace_back(MenuEntity("gady"), gady);
    main.emplace_back(MenuEntity("plazy"), plazy);
    main.emplace_back(MenuEntity("ptaki"), ptaki);

    ssaki.emplace_back(MenuEntity("koty"), koty);
    ssaki.getElement("koty")->getSubMenu()->emplace_back(MenuEntity("bialy", callback));                                                       
    ssaki.getElement("koty")->getSubMenu()->emplace_back(MenuEntity("czarny", callback)); 

    ssaki.emplace_back(MenuEntity("pies", callback)); 
    ssaki.emplace_back(MenuEntity("krowa", callback)); 
    ssaki.emplace_back(MenuEntity("kon", callback)); 

    gady.emplace_back(MenuEntity("krokodyl", callback)); 
    gady.emplace_back(MenuEntity("jaszczurka", callback)); 
    gady.emplace_back(MenuEntity("waz", callback)); 
    gady.emplace_back(MenuEntity("zolw", callback)); 

    plazy.emplace_back(MenuEntity("zaba", callback)); 
    plazy.emplace_back(MenuEntity("ropucha", callback)); 
    plazy.emplace_back(MenuEntity("salamandra", callback)); 

    ptaki.emplace_back(MenuEntity("mewa"));
    ptaki.emplace_back(MenuEntity("golab"));
    ptaki.emplace_back(MenuEntity("orzel"));
    ptaki.emplace_back(MenuEntity("sroka"));

    my_prompt.init(&main);
    
    my_prompt.run();

    return 0;
}