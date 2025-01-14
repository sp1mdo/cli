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
using MenuEntity = std::pair<std::string, Callback>;

enum class FnKey
{
    F1 = 0,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    UP,    // causing unclear behaviour if removed
    DOWN,  // causing unclear behaviour if removed
    LEFT,  // causing unclear behaviour if removed
    RIGHT, // causing unclear behaviour if removed
    LAST_ITEM,
};

class Prompt
{
public:
    Prompt(const std::string &name) :  special_state(false), m_Name(name), m_HistoryIndex(-1), m_SpecialCharsHandling(true), m_oldInput("dummy")
    {
        setNonCanonicalMode();
        updateAuxMenu("");
        m_FunctionKeys[static_cast<int>(FnKey::F1)] = "\x1b\x4f\x50";
        m_FunctionKeys[static_cast<int>(FnKey::F2)] = "\x1b\x4f\x51";
        m_FunctionKeys[static_cast<int>(FnKey::F3)] = "\x1b\x4f\x52";
        m_FunctionKeys[static_cast<int>(FnKey::F4)] = "\x1b\x4f\x53";

#if PICO_ON_DEVICE
        m_FunctionKeys[static_cast<int>(FnKey::F5)] = "\x1b\x5b\x31\x36\x7e";
#else
        m_FunctionKeys[static_cast<int>(FnKey::F5)] = "\x1b\x5b\x31\x35\x7e";
#endif
        m_FunctionKeys[static_cast<int>(FnKey::F6)] = "\x1b\x5b\x31\x37\x7e";
        m_FunctionKeys[static_cast<int>(FnKey::F7)] = "\x1b\x5b\x31\x38\x7e";
        m_FunctionKeys[static_cast<int>(FnKey::F8)] = "\x1b\x5b\x31\x39\x7e";

        m_FunctionKeys[static_cast<int>(FnKey::F9)] = "\x1b\x5b\x32\x30\x7e";
        m_FunctionKeys[static_cast<int>(FnKey::F10)] = "\x1b\x5b\x32\x31\x7e";
        m_FunctionKeys[static_cast<int>(FnKey::F11)] = "\x1b\x5b\x32\x33\x7e";
        m_FunctionKeys[static_cast<int>(FnKey::F12)] = "\x1b\x5b\x32\x34\x7e";

        for (auto &element : m_FnKeyCallback)
        {
            element = nullptr;
        }
    };

    void spin_loop(void)
    {
        updateAuxMenu("");
        print();
        while (1)
        {
            handleKey();
        }
    }
    void handleKey(void);
    void push_back(char c);
    bool backspace(void);
    void print(void) noexcept;
    int try_match(void);
    void parseCommand(void);
    void insertMapElement(std::string &&str, Callback cb);
    void setNonCanonicalMode(void);
    void updateAuxMenu(const std::string &prefix);
    void setSpecialCharsHandling(bool flag);
    bool isSpecialCharsHandlingEnabled(void);

    std::map<std::string, Callback> m_MainMenu;
    std::map<std::string_view, Callback> m_AuxMenu;

    void attachFnKeyCallback(FnKey key, const std::function<void()> &cb);

    std::array<std::function<void()>, static_cast<int>(FnKey::F12) + 1> m_FnKeyCallback;
    std::array<std::string, static_cast<int>(FnKey::LAST_ITEM)> m_FunctionKeys;

private:
    bool special_state;
    std::string m_Input;
    std::string m_Prefix;
    std::string m_Name;
    Tokens m_CommandHistory;
    int m_HistoryIndex;
    bool m_SpecialCharsHandling;
    std::string m_oldInput;

    bool handleSpecialCharacters(void);
    void removeLastWord(std::string &str);
    size_t countCommonPrefixLength(const std::vector<std::string_view> &stringSet);
    size_t countCharacterOccurrences(const std::string &input, char target);
    std::string_view getLastWord(const std::string_view &input);
    void clear_line_back(size_t chars);
    void clear_line_fwd(size_t chars);
    void debug(void);
    std::string getFirstNWords(const std::string &input, size_t N);
};
