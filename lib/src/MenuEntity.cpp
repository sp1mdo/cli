#include <string>
#include <functional>
#include <cstring>
#include "MenuEntity.hpp"
#include "Menu.hpp"

MenuEntity::MenuEntity(std::string label) : m_Label(label), m_Submenu(nullptr) {

                                            };

MenuEntity::MenuEntity(std::string label, std::function<void(std::vector<int>)> fun) : m_Function(fun), m_Label(label)
{
}

std::string MenuEntity::getLabel(void)
{
    return m_Label;
}

void MenuEntity::Function(const Tokens &args)
{
    auto myArgs = TokensToArgv(args);
    std::vector<int> vect_args;
    for(auto & str : args)
    {
        vect_args.push_back(atoi(str.c_str()));
    }

    if (m_Function)
        m_Function(vect_args);

    freeArgv(myArgs, args.size());
}

char **MenuEntity::TokensToArgv(const Tokens &args)
{
    // Allocate an array of char* with size args.size() + 1 (for the NULL terminator)
    char **argv = new char *[args.size() + 1];

    for (size_t i = 0; i < args.size(); ++i)
    {
        // Allocate memory for each string and copy its content
        argv[i] = new char[args[i].size() + 1]; // +1 for the null terminator
        std::strcpy(argv[i], args[i].c_str());
    }

    // Set the last element to NULL to signify the end of the array
    argv[args.size()] = nullptr;

    return argv;
}

void MenuEntity::freeArgv(char **argv, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;
}

bool MenuEntity::operator==(const MenuEntity &other) const
{
    return (m_Label == other.m_Label);
}

void MenuEntity::setSubMenu(Menu *submenu)
{
    m_Submenu = submenu;
}

Menu *MenuEntity::getSubMenu(void)
{
    return m_Submenu;
}
