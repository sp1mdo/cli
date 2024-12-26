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
using Callback = std::function<void(const std::string &)>;

class Prompt
{
public:
    Prompt(const std::string &name) : m_Name(name) , m_HistoryIndex(-1)
        {
            setNonCanonicalMode();
        };

    void run(void);
    void push_back(char c);
    bool backspace(void);
    void print(void);
    int try_match(void);
    void parseCommand(void);

    void insertMapElement(std::string &&str, Callback cb);
    std::map<std::string, Callback> m_MainMenu;
    std::map<std::string, Callback> m_AuxMenu;
    void setNonCanonicalMode(void);
    void updateAuxMenu(const std::string &preix);
private:
    std::string m_Input;
    std::string m_Prefix;
    std::string m_Name;
    Tokens m_CommandHistory;
    int m_HistoryIndex;
    bool handleSpecialCharacters(void);
    size_t countCharacterOccurrences(const std::string &input, char target);
    std::string getLastWord(const std::string &input);
    Tokens tokenize(const std::string &str);
    std::string tokensToString(Tokens &tokens, bool space); // not used ?
    std::string printTokens(const Tokens &tokens);
    void clear_line(size_t chars);
    void debug(void);
    std::string getFirstNWords(const std::string &input, size_t N);

};
