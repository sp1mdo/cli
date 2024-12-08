#pragma once
#include <string>
#include <functional>
#include "Menu.hpp"

class Menu;

using Tokens = std::vector<std::string>;

class MenuEntity
{
public:
    MenuEntity(std::string label);
    MenuEntity(std::string label, std::function<void(int, char **)> fun);
    ~MenuEntity() {};
    std::string getLabel(void);
    void Function(const Tokens &args);
    bool operator==(const MenuEntity &other) const;
    void setSubMenu(Menu *submenu);
    Menu *getSubMenu(void);

private:
    char **TokensToArgv(const Tokens &args);
    void freeArgv(char **argv, size_t size);
    std::function<void(int, char **)> m_Function;
    std::string m_Label;
    Menu *m_Submenu;
};