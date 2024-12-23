#pragma once
#include <vector>
#include <string>
#include <map>
#include <functional>

#ifdef WIN32
#include <conio.h>
#elif UNIX
#include <termios.h>
#include <unistd.h>
#endif

using Tokens = std::vector<std::string>;
using Callback = std::function<void(std::string)>;

class Prompt
{
public:
    Prompt() {};

    void run(void);
    void push_back(char c);
    bool backspace(void);
    void print(void);
    int try_match(void);
    void parseCommand(void);

    void insertMapElement(std::string &&str, Callback cb);
    std::map<std::string, Callback> m_MapMenu;
    void setNonCanonicalMode(void);

private:
    std::string m_Input;

    Tokens tokenize(const std::string &str);
    std::string tokensToString(Tokens &tokens, bool space);
    std::string printTokens(const Tokens &tokens);
    void clear_line(size_t chars);
    void debug(void);
    std::string getFirstNWords(const std::string &input, size_t N);

};
