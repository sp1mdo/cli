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

#define GREEN_COLOR "\033[32m"
#define DEFAULT_COLOR "\033[0m"

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
        // clear_line();
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
        // clear_line();
        return;
    }
}

int try_match(const std::string &input, int *index, int *chars_matching, Menu *current_menu)
{
    int match_count = 0;
    std::vector<int> matching_indexes;
    char next_letter = 0;

    for (int i = 0; i < current_menu->m_Entities.size(); i++)
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
        for (size_t i = 0; i < matching_indexes.size(); i++)
        {
            printf("\t%s\n", current_menu->m_Entities[matching_indexes[i]].getLabel().c_str());
        }
    }

    return match_count;
}

void printTokens(const std::vector<std::string> &tokens)
{
    for (auto &token : tokens)
    {
        printf("%s:", token.c_str());
    }
    printf("\n");
}

std::vector<std::string> tokenize(const std::string &str)
{
    char *token = strtok((char *)str.c_str(), " - ");
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

void clear_line(size_t chars)
{
    for (size_t i = 0; i < chars; i++)
        printf(" ");
    for (size_t i = 0; i < chars; i++)
        printf("\b");
}

void prompt_fun(const std::string &prompt, const std::string &input)
{
    if (prompt.empty())
        printf("\r# %s", input.c_str());
    else
        printf("\r# %s%s%s %s", GREEN_COLOR, prompt.c_str(), DEFAULT_COLOR, input.c_str());
}

std::string find_way_home(Menu* menu)
{
    std::string label = menu->m_Label;
    Menu* parentMenu = menu;
    while(parentMenu->getParent() != nullptr)
    {
        parentMenu = parentMenu->getParent();
        if(parentMenu->getParent() != nullptr)
        {
            label.insert(0, std::string(" "));
            label.insert(0, parentMenu->m_Label);
        }
    }

    return label;
}

int main(int argc, char **argv)
{
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
    ssaki.getElement("koty")->getSubMenu()->emplace_back(MenuEntity("bialy", []()
                                                                    { printf("\nkarbon\n"); }));
    ssaki.getElement("koty")->getSubMenu()->emplace_back(MenuEntity("czarny", []()
                                                                    { printf("\nkokos\n"); }));

    ssaki.emplace_back(MenuEntity("pies", []()
                                  { printf("\nhau\n"); }));
    ssaki.emplace_back(MenuEntity("krowa", []()
                                  { printf("\nmuu\n"); }));
    ssaki.emplace_back(MenuEntity("kon", []()
                                  { printf("\nihaha\n"); }));

    gady.emplace_back(MenuEntity("krokodyl", []()
                                 { printf("\njestem krokodylem\n"); }));
    gady.emplace_back(MenuEntity("jaszczurka", []()
                                 { printf("\njestem jaszczurka\n"); }));
    gady.emplace_back(MenuEntity("waz", []()
                                 { printf("\nssssss\n"); }));
    gady.emplace_back(MenuEntity("zolw", []()
                                 { printf("\njestem zolw\n"); }));

    plazy.emplace_back(MenuEntity("zaba", []()
                                  { printf("\nkum kum\n"); }));
    plazy.emplace_back(MenuEntity("ropucha", []()
                                  { printf("\nrebek rebek\n"); }));
    plazy.emplace_back(MenuEntity("salamandra", []()
                                  { printf("\njestem salamandra\n"); }));

    ptaki.emplace_back(MenuEntity("mewa"));
    ptaki.emplace_back(MenuEntity("golab"));
    ptaki.emplace_back(MenuEntity("orzel"));
    ptaki.emplace_back(MenuEntity("sroka"));

    set_non_canonical_mode();
    std::string input;
    size_t buffer_index = 0;
    int ret = 0;
    std::string prompt;
    Menu *current_menu = &main;
    // Menu *current_menu = &ptaki;
    while (1)
    {
        prompt_fun(prompt, input);
        char z = getc(stdin);
        {
            // printf("\r%02x\n", z);
            // continue;
            if (z == 0x1b) // esc
            {
                prompt.clear();
                input.clear();
                prompt_fun(prompt, input);
                clear_line(20);
                current_menu = &main;
                continue;
            }
            if (z != 9) // tab
            {
                // input.insert(index++, 1, z);
                input.push_back(z);
                buffer_index++;
                // handle_special_characters(buffer, &buffer_index);
            }

            if (z == 0x7f) // backspace
            {
                if (prompt.empty() == false)
                {
                    input = prompt + " " + input + " ";
                    prompt.clear();
                    buffer_index = input.size();
                    prompt_fun(prompt, input);
                    current_menu = current_menu->getParent();
                    printf("  ");
                }

                printf("\b \b");
                if (input.size() > 0)
                {
                    --buffer_index;
                    input.pop_back();
                }
                if (input.size() > 0)
                {
                    --buffer_index;
                    input.pop_back();
                }

                clear_line(20);
                continue;
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
                prompt_fun(prompt, input);
                if (input.size() > 2)
                {
                    std::string new_str = input;
                    new_str.pop_back();
                    if (current_menu->getElement(new_str) != nullptr && current_menu->getElement(new_str)->getSubMenu() != nullptr)
                    {
                        // printf("going to (%s)\n",new_str.c_str());
                        current_menu = current_menu->getElement(new_str)->getSubMenu();
                        if(prompt.empty()) prompt = new_str;
                        else prompt = prompt + " " + new_str;
                        input.clear();
                        continue;
                    }
                }
            }

            // handle_special_chars(index, input);
            if (z == 10) // newline //10 in Linux
            {
                if (!input.empty())
                    input.pop_back();
                if (!input.empty())
                    input.pop_back();
                buffer_index = 0;
                // printTokens(tokenize(input));
                if (current_menu->getElement(input))
                    current_menu->getElement(input)->Function();

                
                input.clear();
                printf("\n\r");
            }
        }
    }
}