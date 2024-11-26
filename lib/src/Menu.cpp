#include <string>
#include <vector>
#include "MenuEntity.hpp"
#include "Menu.hpp"

Menu::Menu(const std::string &newlabel) : m_Label(newlabel)
{
}

MenuEntity* Menu::getElement(std::string label)
{
    for (auto &element : m_Entities)
    {
        if (element.getLabel() == label)
            return &element;
    }

    return nullptr;
}

void Menu::printEntities(std::string &prefix)
{
    for (auto &entity : m_Entities)
    {
        if (entity.getLabel().find(prefix) == 0)
            printf("\t%s\n", entity.getLabel().c_str());
    }
}
