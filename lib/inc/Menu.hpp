#pragma once
#include <string>
#include <vector>
#include "MenuEntity.hpp"
class MenuEntity;

class Menu
{
public:
    Menu(const std::string &newlabel);
    void printEntities(std::string &prefix);

    std::vector<MenuEntity> m_Entities;
    std::string m_Label;
    MenuEntity *getElement(std::string label);
    void setParent(Menu* parentMenu);
    Menu* getParent(void);
    MenuEntity& emplace_back(MenuEntity && other);

private:
    Menu* m_Parent;
};