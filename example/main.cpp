#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <set>
#include <termios.h>
#include <unistd.h>
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

void set_non_canonical_mode()
{
    struct termios newt, oldt;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);

    // Make a copy of the settings to modify
    newt = oldt;

    // Disable canonical mode (line buffering) and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;  // Minimum number of characters to read (1 character)
    newt.c_cc[VTIME] = 0; // No timeout

    // Apply the new terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

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

void callback(int argc, char** argv)
{
    printf("Received : ");
    for(size_t i = 0 ; i < argc ; i++)
    {
        printf("%s, ",argv[i]);
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
    ssaki.getElement("koty")->getSubMenu()->emplace_back(MenuEntity("bialy", [](int var, char** argv){printf("podano %d\n",var);}));                                                          
    ssaki.getElement("koty")->getSubMenu()->emplace_back(MenuEntity("czarny", std::bind(callback, 1, std::placeholders::_2)));    

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

    set_non_canonical_mode();
    Menu *current_menu = &main;

    my_prompt.setMenu(&main);
    
    while (1)
    {
        my_prompt.print();
        char z = getc(stdin);
        {
            // printf("\r%02x\n", z); // debug purpose
            // continue;

            if (z == 0x1b) // esc
            {
                my_prompt.goToRoot();
                continue;
            }
            if (z != 9 && z != 0x7f) // any valid char, but not tab
            {
                my_prompt.push_back(z);
                my_prompt.print();
            }

            if (z == 0x7f) // backspace
            {
                my_prompt.backspace();
                continue;
            }
            else if (z == 9) // tab
            {
                my_prompt.try_match();
            }

            // handle_special_chars(index, input);
            if (z == 10) // newline //10 in Linux
            {
                my_prompt.parseCommand();
                my_prompt.print();
            }
        }
    }
}