#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <set>
#include <termios.h>
#include <unistd.h>
#include <cstring>

#define STR_TO_LOWER(str)                     \
    do                                        \
    {                                         \
        for (int i = 0; i < strlen(str); i++) \
            str[i] = tolower(str[i]);         \
    } while (0);

const char *left_key = "\x1b\x5b\x44";
const char *right_key = "\x1b\x5b\x43";

class Menu;

void clear_line(void)
{
    for (size_t i = 0; i < 40 + 2; i++)
        printf("\b");
    for (size_t i = 0; i < 40 + 2; i++)
        printf(" ");
    for (size_t i = 0; i < 40 + 2; i++)
        printf("\b");
}

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

class MenuEntity
{
public:
    MenuEntity(std::string label) : m_Label(label) {};

    // MenuEntity(std::string &label, std::function<void()> func) : m_Label(label), m_Function(func) {};

    ~MenuEntity() {};

    std::string getLabel(void)
    {
        return m_Label;
    }

    std::function<void()> Function(void)
    {
        return m_Function;
    }

    bool operator==(const MenuEntity &other) const
    {
        // Compare the string and the result of the function
        return (m_Label == other.m_Label);
    }

private:
    std::string m_Label;
    std::function<void()> m_Function;
    Menu *m_Submenu;
};

class Menu
{

public:
    void printEntities(std::string &prefix)
    {
        for (auto &entity : m_Entities)
        {
            if (entity.getLabel().find(prefix) == 0)
                printf("\t%s\n", entity.getLabel().c_str());
        }
    }

    std::vector<MenuEntity> m_Entities;

    MenuEntity *getElement(std::string label)
    {
        for (auto &element : m_Entities)
        {
            if (element.getLabel() == label)
                return &element;
        }

        return nullptr;
    }
};

void handle_special_chars(size_t &index, std::string &input)
{
    if (input == std::string(left_key))
    {
        for (size_t i = 0; i < sizeof(left_key); i++)
            ;
        input.pop_back();

        if (index > 0)
        {
            index--;
            printf("\b");
        }
        clear_line();
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
        clear_line();
        return;
    }
}

int try_match(std::string &input, int *index, int *chars_matching, Menu *current_menu)
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

size_t howMuchTheSame(const std::string &prefix, const std::string &base)
{
    size_t i = 0;
    for (i = 0; i < prefix.size(); i++)
    {
        if (prefix.at(i) == base.at(i))
            i++;
        else
            break;
    }
    return i;
}

int main(int argc, char **argv)
{
    Menu main;

    // MenuEntity entajty = MenuEntity(std::string("Ala"));
    main.m_Entities.emplace_back(MenuEntity("Ala"));
    main.m_Entities.emplace_back("Ela");
    main.m_Entities.emplace_back("Urszula");
    main.m_Entities.emplace_back("Elzbieta");
    main.m_Entities.emplace_back("Marek");
    main.m_Entities.emplace_back("Natala");
    main.m_Entities.emplace_back("Emilia");
    main.m_Entities.emplace_back("Piotrek");
    main.m_Entities.emplace_back("Monika");
    main.m_Entities.emplace_back("Lukasz");
    main.m_Entities.emplace_back("Mateusz");
    main.m_Entities.emplace_back("Lechoslaw");
    main.m_Entities.emplace_back("Anna");
    main.m_Entities.emplace_back("Grazyna");
    main.m_Entities.emplace_back("Adam");
    main.m_Entities.emplace_back("Leszek");
    main.m_Entities.emplace_back("Marcin");

    // printf("%lu\n", howMuchTheSame("Ma","Marek"));

    // std::string prefix = std::string(argv[1]);
    // main.printEntities(prefix);

    set_non_canonical_mode();
    std::string input;
    size_t buffer_index = 0;
    int ret = 0;
    while (1)
    {
        if (buffer_index == 0)
            printf("\r# ");
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
                ret = try_match(input, &index2, &chars_matching, &main);

                if (ret >= 1)
                {
                    // memset(buffer, 0, sizeof(buffer));
                    input.clear();
                    // chars_matching = strlen(buffer);
                    if (ret == 1)
                        input = main.m_Entities[index2].getLabel(); // strcpy(buffer, current_menu[index].command);
                    else
                        input.assign(main.m_Entities[index2].getLabel(), 0, chars_matching); // strncpy(buffer, current_menu[index].command, chars_matching);
                    if (ret == 1)
                        input.push_back(' ');    // buffer[strlen(buffer)] = ' ';
                    buffer_index = input.size(); // buffer_index = strlen(buffer);
                }
                // printf(".");
            }

            if (ret > 0 || z != 9)
            {
                // if(strcmp(old_buf, buffer) != 0)
                // {
                printf("\r# ");
                printf("%s", input.c_str());
                // strncpy(old_buf, buffer, TERMINAL_BUFF_SIZE);
                //}
            }
            // handle_special_chars(index, input);
            if (z == 10) // newline //10 in Linux
            {
                input.pop_back();
                buffer_index = 0; //--;//[--buffer_index] = 0; // string null termination
                // newline
                // buffer_index = 0;
                input.clear(); // memset(old_buf, 0, sizeof(old_buf));
                printf("\n\r");

                // return buffer;
            }
        }
    }
}