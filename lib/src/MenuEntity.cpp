#include <string>
#include <functional>
#include "MenuEntity.hpp"
#include "Menu.hpp"

MenuEntity::MenuEntity(std::string label) : m_Label(label), m_Submenu(nullptr) {};

MenuEntity::MenuEntity(std::string label, std::function<void()> fun) :  m_Function(fun), m_Label(label)
{

}
std::string MenuEntity::getLabel(void)
{
    return m_Label;
}

void MenuEntity::Function(void)
{
    m_Function();
}

bool MenuEntity::operator==(const MenuEntity &other) const
{
    // Compare the string and the result of the function
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
