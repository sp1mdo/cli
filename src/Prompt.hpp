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
        Prompt(std::string_view name);

        Prompt(const Prompt &other) = delete;
        Prompt(Prompt &&other) = delete;
        Prompt &operator=(const Prompt &) = delete;
        Prompt &operator=(Prompt &&) = delete;

        void run(void);
        void insertMenuItem(const std::string &str, const Callback &cb);
        void emplaceMenuItem(std::string &&str, Callback &&cb);
        void attachFnKeyCallback(FnKey key, const std::function<void()> &cb);

    private:
        std::map<std::string, Callback> m_MainMenu;
        std::map<std::string_view, Callback> m_AuxMenu;
        std::array<std::function<void()>, static_cast<int>(FnKey::F12) + 1> m_FnKeyCallback;

        bool m_specialCharacterState;
        std::string m_Input;
        std::string m_Prefix;
        std::string m_Name;
        Tokens m_CommandHistory;
        int m_historyIndex;
        std::string m_oldInput;
        std::size_t m_LongestCommand;

        void handleKey(void);
        bool backspace(void);
        void print(void) noexcept;
        int tryMatch(void);
        void parseCommand(void);

        template <typename T>
        void addUnique(std::vector<T> &uniqueVector, T &element) const;
        std::string_view getNwords(const std::string &substr, std::string_view str) const;
        std::size_t getMaxCommandLength(const std::vector<std::string> &commands) const;
        void setNonCanonicalMode(void) const noexcept;
        void updateAuxMenu(const std::string &prefix = {});
        bool handleSpecialCharacters(void);
        void removeLastWord(std::string &str) const;
        std::size_t countCommonPrefixLength(const std::vector<std::string_view> &stringSet) const;
        std::size_t countCharacterOccurrences(const std::string &input, char target) const;
        std::string_view getLastWord(std::string_view input) const;
        void clearLineBack(std::size_t chars) const noexcept;
        void clearLineFwd(std::size_t chars) const noexcept;
        void debug(void) const noexcept;
    };
}

#endif // CLI_H