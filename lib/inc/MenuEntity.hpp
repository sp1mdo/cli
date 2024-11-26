#pragma once
#include <string>
#include <functional>
#include "Menu.hpp"
class Menu;

class MenuEntity
{
public:
    MenuEntity(std::string label) ;
    ~MenuEntity() {};
    std::string getLabel(void);
    std::function<void()> Function(void);
    bool operator==(const MenuEntity &other) const;
    void setSubMenu(Menu *submenu);
    Menu *getSubMenu(void);

private:
    std::function<void()> m_Function;
    std::string m_Label;
    Menu *m_Submenu;
};