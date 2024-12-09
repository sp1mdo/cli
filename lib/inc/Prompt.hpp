#pragma once
#include <vector>
#include <string>
#include "Menu.hpp"

#ifdef WIN32
#include <conio.h>
#elif UNIX
#include <termios.h>
#include <unistd.h>
#endif


using Tokens = std::vector<std::string>;

class Prompt
{
public:
    Prompt() {};

    void run(void);
    void push_back(char c);
    bool backspace(void);
    void organize(char c);
    void print(void);
    int try_match(void);
    std::string *getInput(void);
    void parseCommand(void);
    void init(Menu *menu);
    void goToRoot(void);

private:
    std::string m_Input;
    Menu *m_CurrentMenu;
    Tokens tokenize(const std::string &str);
    void setNonCanonicalMode(void);
    std::string tokensToString(Tokens &tokens, bool space);
    std::string printTokens(const Tokens &tokens);
    void clear_line(size_t chars);
    void debug(void);

    Tokens m_Prefix;
};
