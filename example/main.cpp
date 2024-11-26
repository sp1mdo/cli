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

#define STR_TO_LOWER(str)                     \
    do                                        \
    {                                         \
        for (int i = 0; i < strlen(str); i++) \
            str[i] = tolower(str[i]);         \
    } while (0);

const char *left_key = "\x1b\x5b\x44";
const char *right_key = "\x1b\x5b\x43";

class Menu;

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
        //clear_line();
        //

        return;
    }
    if (input == std::string(right_key))
    {
        for (size_t i = 0; i < sizeof(right_key); i++)
            ;
        input.pop_back();
        if (index < input.size())
            index++;
        //clear_line();
        return;
    }
}

int try_match(const std::string &input, int *index, int *chars_matching, Menu *current_menu)
{
    int match_count = 0;
    std::vector<int> matching_indexes;
    char next_letter = 0;

    for (int i = 0; i < current_menu->m_Entities.size() - 1; i++)
    {
        if (current_menu->m_Entities[i].getLabel().find(input) == 0)
        {
            matching_indexes.push_back(i);
            *index = i;
            match_count++;
            *chars_matching = input.size();
        }
    }

    if (matching_indexes.size() > 0)
    {
        printf("\n");
        for (uint32_t i = 0; i < matching_indexes.size(); i++)
        {
            printf("\t%s\n", current_menu->m_Entities[matching_indexes[i]].getLabel().c_str());
        }
    }

    return match_count;
}

void printTokens(const std::vector<std::string> &tokens)
{
    for(auto & token : tokens)
    {
        printf("%s:",token.c_str());
    }
    printf("\n");
}

std::vector<std::string> tokenize(const std::string &str)
{
    char *token = strtok((char*)str.c_str(), " - ");
    std::vector<std::string> tokens;
    while (token != NULL)
    {
        int arg_value;
        sscanf(token, "%d", &arg_value);
        tokens.push_back(std::string(token));
        token = strtok(NULL, " ");
    }
    return tokens;
}

int main(int argc, char **argv)
{
    Menu main("main");

    // MenuEntity entajty = MenuEntity(std::string("Ala"));
    main.m_Entities.emplace_back(MenuEntity("ssaki"));
    main.m_Entities.emplace_back("gady");
    main.m_Entities.emplace_back("plazy");
    main.m_Entities.emplace_back("ryby");
    main.m_Entities.emplace_back("ptaki");
    
    Menu ssaki("ssaki");
    ssaki.m_Entities.emplace_back("kot");
    ssaki.m_Entities.emplace_back("pies");
    ssaki.m_Entities.emplace_back("krowa");
    ssaki.m_Entities.emplace_back("kon");

    Menu gady("gady");
    gady.m_Entities.emplace_back("krokodyl");
    gady.m_Entities.emplace_back("jaszczurka");
    gady.m_Entities.emplace_back("waz");
    gady.m_Entities.emplace_back("zolw");

    Menu plazy("plazy");
    plazy.m_Entities.emplace_back("zaha");
    plazy.m_Entities.emplace_back("ropucha");
    plazy.m_Entities.emplace_back("salamandra");

    Menu ryby("ryby");
    ryby.m_Entities.emplace_back("okon");
    ryby.m_Entities.emplace_back("szczupak");
    ryby.m_Entities.emplace_back("leszcz");
    ryby.m_Entities.emplace_back("jazgarz");

    Menu ptaki("ptaki");
    ptaki.m_Entities.emplace_back("mewa");
    ptaki.m_Entities.emplace_back("golab");
    ptaki.m_Entities.emplace_back("orzel");
    ptaki.m_Entities.emplace_back("sroka");

    main.getElement("ssaki")->setSubMenu(&ssaki);
    main.getElement("gady")->setSubMenu(&gady);
    main.getElement("plazy")->setSubMenu(&plazy);
    main.getElement("ryby")->setSubMenu(&ryby);
    main.getElement("ptaki")->setSubMenu(&ptaki);


    set_non_canonical_mode();
    std::string input;
    size_t buffer_index = 0;
    int ret = 0;

    Menu *current_menu = &main;
    //Menu *current_menu = &ptaki;
    while (1)
    {
        if (buffer_index == 0)
            printf("\r%s # ", current_menu->m_Label.c_str());
        char z = getc(stdin);
        {
            // printf("\r%02x\n", z);
            // continue;

            if (z != 9) // tab
            {
                // input.insert(index++, 1, z);
                input.push_back(z);
                buffer_index++;
                // handle_special_characters(buffer, &buffer_index);
            }

            if (z == 0x7f) // backspace
            {
                printf("\b \b");
                if (buffer_index > 0)
                {
                    --buffer_index;
                    input.pop_back();
                }
                if (buffer_index > 0)
                {
                    --buffer_index;
                    input.pop_back();
                }
            }
            else if (z == 9) // tab
            {
                int index2 = -1;
                int chars_matching = -1;
                // STR_TO_LOWER(buffer);
                ret = try_match(input, &index2, &chars_matching, current_menu);

                if (ret >= 1)
                {
                    input.clear();
                    if (ret == 1)
                        input = current_menu->m_Entities[index2].getLabel();
                    else
                        input.assign(current_menu->m_Entities[index2].getLabel(), 0, chars_matching);
                    if (ret == 1)
                        input.push_back(' ');
                    buffer_index = input.size();
                }
            }

            if (ret > 0 || z != 9)
            {
                printf("\r%s # ", current_menu->m_Label.c_str());
                printf("%s", input.c_str());
              if(input.size() > 2)  
              {  std::string new_str = input;
                new_str.pop_back();
                if(current_menu->getElement(new_str) != nullptr && current_menu->getElement(new_str)->getSubMenu() != nullptr)
                {
                  //printf("going to (%s)\n",new_str.c_str());
                    current_menu = current_menu->getElement(new_str)->getSubMenu();
                }
            }  
            }

            // handle_special_chars(index, input);
            if (z == 10) // newline //10 in Linux
            {
                input.pop_back();
                buffer_index = 0;
                //printTokens(tokenize(input));
                input.clear();
                printf("\n\r");
            }
        }
    }
}