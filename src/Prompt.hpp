#ifndef CLI_H
#define CLI_H

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

namespace cli
{
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
        LAST_ITEM,
    };

    class Prompt
    {
    public:
        Prompt(const std::string &name);

        Prompt(const Prompt &other) = delete;
        Prompt(Prompt &&other) = delete;
        Prompt &operator=(const Prompt &) = delete;
        Prompt &operator=(Prompt &&) = delete;

        void Run(void);
        void insertMenuItem(std::string &&str, Callback cb);
        void attachFnKeyCallback(FnKey key, const std::function<void()> &cb);

    private:
        std::map<std::string, Callback> m_MainMenu;
        std::map<std::string_view, Callback> m_AuxMenu;
        std::array<std::function<void()>, static_cast<int>(FnKey::F12) + 1> m_FnKeyCallback;

        bool m_SpecialCharacterState;
        std::string m_Input;
        std::string m_Prefix;
        std::string m_Name;
        Tokens m_CommandHistory;
        int m_HistoryIndex;
        std::string m_oldInput;
        size_t m_LongestCommand;

        void handleKey(void);
        bool backspace(void);
        void print(void) noexcept;
        int tryMatch(void);
        void parseCommand(void);

        std::string_view getNwords(const std::string &substr, std::string_view str) const;
        size_t getMaxCommandLength(const std::vector<std::string> &commands) const;
        void setNonCanonicalMode(void) const noexcept;
        void updateAuxMenu(const std::string &prefix);
        bool handleSpecialCharacters(void);
        void removeLastWord(std::string &str) const;
        size_t countCommonPrefixLength(const std::set<std::string_view> &stringSet) const;
        size_t countCharacterOccurrences(const std::string &input, char target) const;
        std::string_view getLastWord(std::string_view input) const;
        void clear_line_back(size_t chars) const noexcept;
        void clear_line_fwd(size_t chars) const noexcept;
        void debug(void) const noexcept;
    };
}

#endif // CLI_H