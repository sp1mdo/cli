#include <string>
#include <functional>
#include "MenuEntity.hpp"
#include "Menu.hpp"

MenuEntity::MenuEntity(std::string label) : m_Label(label), m_Submenu(nullptr) {};

std::string MenuEntity::getLabel(void)
{
    return m_Label;
}

std::function<void()> MenuEntity::Function(void)
{
    return m_Function;
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
