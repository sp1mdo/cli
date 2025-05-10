#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <set>
#include <cstdint>
#include "Prompt.hpp"

#if defined PICO_ON_DEVICE
#include "pico/time.h"
#include "pico/error.h"
#include "pico/stdio.h"
#endif

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#define UNIX
#elif defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

#ifdef UNIX
#include <termios.h>
#include <unistd.h>
#endif

#ifdef WINDOWS
#include <conio.h> // For _getch()
#endif

#define GREEN_COLOR "\033[32m"
#define CYAN_COLOR "\033[36m"
#define DEFAULT_COLOR "\033[0m"

#define DEBUG 0

using namespace cli;

static constexpr size_t CLEAR_BACK_CHARS{20};

static constexpr std::string_view UP_KEY1{"\x1b\x5b\x41"};
static constexpr std::string_view UP_KEY2{"\x1b\x4f\x41"};
static constexpr std::string_view DOWN_KEY1{"\x1b\x5b\x42"};
static constexpr std::string_view DOWN_KEY2{"\x1b\x4f\x42"};
static constexpr std::string_view LEFT_KEY1{"\x1b\x5b\x44"};
static constexpr std::string_view LEFT_KEY2{"\x1b\x4f\x44"};
static constexpr std::string_view RIGHT_KEY1{"\x1b\x5b\x43"};
static constexpr std::string_view RIGHT_KEY2{"\x1b\x4f\x43"};

static constexpr std::string_view F1_KEY1{"\x1b\x4f\x50"};
static constexpr std::string_view F1_KEY2{"\x1b\x5b\x31\x31\x7e"}; // done
static constexpr std::string_view F2_KEY1{"\x1b\x4f\x51"};
static constexpr std::string_view F2_KEY2{"\x1b\x5b\x31\x32\x7e"};
static constexpr std::string_view F3_KEY1{"\x1b\x4f\x52"};
static constexpr std::string_view F3_KEY2{"\x1b\x5b\x31\x33\x7e"};
static constexpr std::string_view F4_KEY1{"\x1b\x4f\x53"};
static constexpr std::string_view F4_KEY2{"\x1b\x5b\x31\x34\x7e"};
static constexpr std::string_view F5_KEY1{"\x1b\x5b\x31\x36\x7e"};
static constexpr std::string_view F5_KEY2{"\x1b\x5b\x31\x35\x7e"};
static constexpr std::string_view F5_KEY3{"\x1b\x4f\x54"};
static constexpr std::string_view F6_KEY1{"\x1b\x5b\x31\x37\x7e"};
static constexpr std::string_view F6_KEY2{"\x1b\x4f\x55"};
static constexpr std::string_view F7_KEY1{"\x1b\x5b\x31\x38\x7e"};
static constexpr std::string_view F7_KEY2{"\x1b\x4f\x56"};
static constexpr std::string_view F8_KEY1{"\x1b\x5b\x31\x39\x7e"};
static constexpr std::string_view F8_KEY2{"\x1b\x4f\x57"};
static constexpr std::string_view F9_KEY1{"\x1b\x5b\x32\x30\x7e"};
static constexpr std::string_view F9_KEY2{"\x1b\x4f\x58"};
static constexpr std::string_view F10_KEY1{"\x1b\x5b\x32\x31\x7e"};
static constexpr std::string_view F10_KEY2{"\x1b\x4f\x59"};
static constexpr std::string_view F11_KEY1{"\x1b\x5b\x32\x33\x7e"};
static constexpr std::string_view F11_KEY2{"\x1b\x4f\x5a"};
static constexpr std::string_view F12_KEY1{"\x1b\x5b\x32\x34\x7e"};
static constexpr std::string_view F12_KEY2{"\x1b\x4f\x5b"};

static constexpr char dummy_char = 0x05;

static constexpr size_t history_size{20};

#ifdef UNIX
const char newline_char = 10;
const char backspace_char = 0x7f;
#elif defined PICO_ON_DEVICE
const char newline_char = 13;
const char backspace_char = 0x08;
#endif

static constexpr char UNIX_NEWLINE_CHAR{0x0A};
static constexpr char UNIX_BACKSPACE_CHAR{0x7F};

static constexpr char PICO_NEWLINE_CHAR{0x0D};
static constexpr char PICO_BACKSPACE_CHAR{0x08};

const char tab_char = 0x09;

uint32_t alloc_count = 0;
// 536948456 - max memory before dead

#if DEBUG // Tool, to analyze how certain STL solution is allocation-efficient
void *operator new(size_t size) noexcept
{
    void *p = malloc(size);
    alloc_count++;
    return p;
}

void operator delete(void *p) noexcept
{
    free(p);
}

#endif

void dumpString(const std::string &str)
{
    printf("\n[");
    for (auto &xchar : str)
    {
        printf("%02x ", xchar);
    }
    printf("]\n");
}

// Variadic template function to check equality with multiple values
template <typename T, typename... Args>
constexpr bool isEqualToAny(T value, Args... args)
{
    return ((value == args) || ...);
}

Prompt::Prompt(const std::string &name) : m_SpecialCharacterState(false), m_Name(name), m_HistoryIndex(-1), m_oldInput("dummy")
{
    setNonCanonicalMode();
    updateAuxMenu("");

    std::for_each(m_FnKeyCallback.begin(), m_FnKeyCallback.end(), [](auto &element) { element = nullptr; });
};

void Prompt::Run(void)
{
    updateAuxMenu("");
    print();
    while (1)
    {
        handleKey();
    }
}

void Prompt::setNonCanonicalMode(void) const noexcept
{
#ifdef UNIX
    struct termios newt, oldt;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);

    // Make a copy of the settings to modify
    newt = oldt;

    // Disable canonical mode (line buffering) and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;  // Minimum number of characters to read (1 character)
    newt.c_cc[VTIME] = 0; // No timeout

    // Apply the new terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
}

void Prompt::handleKey(void)
{
    bool special_handling = false;
    static uint16_t sum = 1;
    char z = 0;
    print();
#ifdef PICO_ON_DEVICE // specific for Raspberry pico RP2040
    int x = getchar_timeout_us(10);
    if (x == PICO_ERROR_TIMEOUT)
        return;
#elif defined UNIX
    int x = getc(stdin);
#elif defined WINDOWS
    int x = _getch();
#endif

    z = (char)x;
    // printf("\r%02x\n", z); // debug purpose
    // return;
    

    // Pressed some special key that corresponds with sequence of bytes
    if (z == 0x1b && m_SpecialCharacterState == false)
    {
        m_SpecialCharacterState = true;
        m_Input.push_back(z);
        return;
    }

    // Recovery from being stuck in m_SpecialCharacterState
    if (z == 0x1b && m_SpecialCharacterState == true) 
    {
        m_SpecialCharacterState = false;
        m_Input.clear();
        clear_line_back(CLEAR_BACK_CHARS);
        return;
    }

    if ((isEqualToAny(z, PICO_BACKSPACE_CHAR, UNIX_BACKSPACE_CHAR) /* backspace */ && m_Input.empty() == true && m_Prefix.empty() == true))
    {
        // Prefix is empty, just do nothing
        return;
    }

    if ((isEqualToAny(z, PICO_BACKSPACE_CHAR, UNIX_BACKSPACE_CHAR) /* backspace */ && m_Input.empty() == true && m_Prefix.empty() == false))
    {
        m_oldInput = m_Input + dummy_char;
        removeLastWord(m_Prefix);
        updateAuxMenu(m_Prefix);
        m_Input.clear();
        clear_line_back(CLEAR_BACK_CHARS);
        print();
        return;
    }

    if (z != tab_char) // any valid char, but not tab
    {
        m_Input.push_back(z);
        special_handling = handleSpecialCharacters();

        if (special_handling)
        {
            m_SpecialCharacterState = false;
            print();
            clear_line_fwd(50);
            return;
        }
    }
    size_t num = 0;
    if (isEqualToAny(z, UNIX_BACKSPACE_CHAR, PICO_BACKSPACE_CHAR)) // backspace
    {
        backspace();
    }
    else if (z == tab_char) // tab
    {
        num = tryMatch();
        m_oldInput = m_Input + dummy_char; // add dummy char to force printin the prompt
    }

    if (z != tab_char || num > 0 || special_handling)
    {
        sum = 0;
        for (size_t i = 0; i < m_Input.size(); i++)
        {
            sum = sum + m_Input[i];
        }
        print();
    }

    if (isEqualToAny(z, UNIX_NEWLINE_CHAR, PICO_NEWLINE_CHAR)) // newline
    {
#ifdef PICO_ON_DEVICE
        printf("\n");
#endif
        parseCommand();
        print();
    }
}

void Prompt::debug(void) const noexcept
{
    printf("\n[%s] \n", m_Input.c_str());
}

void Prompt::removeLastWord(std::string &str) const
{
    while (str.empty() == false)
    {
        if (str.back() == ' ')
            break;

        str.pop_back();
    }

    if (str.size() > 0)
        str.pop_back();
}

template <typename... Args>
bool containsAny(std::string_view str, const Args &...substrs)
{
    return ((str.find(substrs) != std::string::npos) || ...);
}

bool Prompt::handleSpecialCharacters(void)
{
    // Handle key_up for scrolling the command history
    if (containsAny(m_Input, UP_KEY1, UP_KEY2))
    {
        // Add dummy char to force print the prompt
        m_oldInput = m_Input + dummy_char;

        //Specific for Putty, requires to compensate cursor moving up
        if (m_Input.find(UP_KEY2) != std::string::npos)
            printf("\n");

        m_Input.clear();
        if (m_CommandHistory.empty() == true)
        {
            return true;
        }
        m_HistoryIndex++;
        if (m_HistoryIndex >= 0)
        {
            if (m_HistoryIndex >= (int)m_CommandHistory.size())
                m_HistoryIndex = m_CommandHistory.size() - 1;
        }

        m_Input = m_CommandHistory[m_HistoryIndex];

        clear_line_back(50);
        return true;
    }

    // Handle key_down for scrolling the command history
    if (containsAny(m_Input, DOWN_KEY1, DOWN_KEY2))
    {
        // Add dummy char to force print the prompt
        m_oldInput = m_Input + dummy_char;
        m_Input.clear();
        if (m_CommandHistory.empty() == true)
        {
            m_Input.clear();
            return true;
        }

        m_HistoryIndex--;
        if (m_HistoryIndex < 0)
        {
            m_Input.clear();
            m_HistoryIndex = -1;
        }
        else
        {
            m_Input = m_CommandHistory[m_HistoryIndex];
        }

        return true;
    }

    // Left and Right arrows, just do nothing and prevent from messing the console
    if (containsAny(m_Input, LEFT_KEY1, LEFT_KEY2, RIGHT_KEY1, RIGHT_KEY2))
    {
        m_Input.clear();
        return true;
    }

    ssize_t index = -1;
    // Handle key F1
    if (containsAny(m_Input, F1_KEY1, F1_KEY2)) // done
    {
        index = 0;
    }
    // Handle key F2
    else if (containsAny(m_Input, F2_KEY1, F2_KEY2))
    {
        index = 1;
    }
    // Handle key F3
    else if (containsAny(m_Input, F3_KEY1, F3_KEY2))
    {
        index = 2;
    }
    // Handle key F4
    else if (containsAny(m_Input, F4_KEY1, F4_KEY2))
    {
        index = 3;
    }
    // Handle key F5
    else if (containsAny(m_Input, F5_KEY1, F5_KEY2, F5_KEY3))
    {
        index = 4;
    }
    // Handle key F6
    else if (containsAny(m_Input, F6_KEY1, F6_KEY2))
    {
        index = 5;
    }
    // Handle key F7
    else if (containsAny(m_Input, F7_KEY1, F7_KEY2))
    {
        index = 6;
    }
    // Handle key F8
    else if (containsAny(m_Input, F8_KEY1, F8_KEY2))
    {
        index = 7;
    }
    // Handle key F9
    else if (containsAny(m_Input, F9_KEY1, F9_KEY2))
    {
        index = 8;
    }
    // Handle key F10
    else if (containsAny(m_Input, F10_KEY1, F10_KEY2))
    {
        index = 9;
    }
    // Handle key F11
    else if (containsAny(m_Input, F11_KEY1, F11_KEY2))
    {
        index = 10;
    }
    // Handle key F12
    else if (containsAny(m_Input, F12_KEY1, F12_KEY2))
    {
        index = 11;
    }

    if (index != -1)
    {
        printf("\n "); // this avoids  moving the cursor up
        m_oldInput = m_Input + dummy_char;
        m_Input.clear();
        try
        {
            if (m_FnKeyCallback[index])
                m_FnKeyCallback[index]();
            else
                printf("\nF%zu has no function attached to it.\n", index + 1);
            return true;
        }
        catch(std::exception &e)
        {
            std::cout << "Exception occured:" << e.what();
        }
    }
    return false;
}

bool Prompt::backspace(void) // todo void
{
    printf("\b \b");

    if (m_Input.size() > 0)
    {
        m_Input.pop_back();
    }
    if (m_Input.size() > 0)
    {
        m_Input.pop_back();
    }

    clear_line_back(CLEAR_BACK_CHARS);

    return true;
}

size_t Prompt::getMaxCommandLength(const std::vector<std::string> &commands) const
{
    size_t maxLength = 0;

    for(auto & command : commands)
    {
        if(maxLength < command.size())
            maxLength = command.size();
    }

    return maxLength;
}

void Prompt::parseCommand(void)
{
    while (isEqualToAny(m_Input.back(), ' ', UNIX_NEWLINE_CHAR, PICO_NEWLINE_CHAR)) // trim all newline chars and spaces at the end of input str
        m_Input.pop_back();

    size_t cnt = 0;
    bool last = false;
    bool found = false;

    // Check whether given input is found in command list
    for (auto &element : m_AuxMenu)
    {
        // Check whether this string is the full word
        if (element.first.rfind(m_Input + " ") == 0) // TODO in case this word is also somewhere else this will not work.
        {
            found = true;
        }

        if (element.first.find(m_Input) != std::string::npos)
        {
            cnt++;
            // Don't make prefix if the word is the last one, so it's the command actually
            if (getLastWord(element.first) == m_Input && cnt == 1)
                last = true;
        }
    }

    std::string updatestr;
    if (m_Prefix.empty() == true)
        updatestr = m_Input;
    else
        updatestr = m_Prefix + " " + m_Input;

    // Check if given string matches to move into relative path of the menu
    if (cnt >= 1 && m_Input.empty() == false && last == false && m_AuxMenu.find(m_Input) == m_AuxMenu.end())
    {
        if (found == true)
            updateAuxMenu(updatestr);
        else
            fprintf(stderr, "\nUnknown command\n");

        m_Input.clear();
        clear_line_back(CLEAR_BACK_CHARS);
        return;
    }

    bool executed = false;
    for (size_t i = 0; i < m_Input.size(); i++)
    {
        // Find a place, where the command is separated from the args
        std::string command(m_Input, 0, i + 1);
        if (m_AuxMenu.find(command) != m_AuxMenu.end())
        {
            std::string args(m_Input, i + 1, sizeof(m_Input));
            while (args.front() == ' ')
                args.erase(args.begin());

            // Execute callabck with given args
            printf("\n");
            try
            {
                m_AuxMenu.at(command)(args);
            }
            catch (const std::invalid_argument &e)
            {
                printf("Error: Invalid argument. Input is not a valid number.\n");
            }
            printf("\n");
            executed = true;

            // Add good command to the command history
            if (m_CommandHistory.size() >= 1)
            {
                if (m_CommandHistory[0] != command + " " + args)
                {
                    m_CommandHistory.insert(m_CommandHistory.begin(), command + " " + args);
                }
            }
            else
                m_CommandHistory.insert(m_CommandHistory.begin(), command + " " + args);

            if (m_CommandHistory.size() > history_size) // Limit the command history size
                m_CommandHistory.erase(m_CommandHistory.end());

            m_LongestCommand = getMaxCommandLength(m_CommandHistory);

            m_HistoryIndex = -1;
            break;
        }
    }

    if (cnt == 0 && executed == false)
    {
        fprintf(stderr, "Unknown command\n");
    }

    m_Input.clear();
    clear_line_back(50);
}

void Prompt::clear_line_fwd(size_t chars) const noexcept
{
    for (size_t i = 0; i < chars; i++)
        printf(" ");
    for (size_t i = 0; i < chars; i++)
        printf("\b");
}

void Prompt::clear_line_back(size_t chars) const noexcept
{
    for (size_t i = 0; i < chars; i++)
        printf("\b");
    for (size_t i = 0; i < chars; i++)
        printf(" ");
    for (size_t i = 0; i < chars; i++)
        printf("\b");
}

size_t Prompt::countCharacterOccurrences(const std::string &input, char target) const
{
    return std::count(input.begin(), input.end(), target);
}

std::string_view Prompt::getLastWord(std::string_view input) const
{
    std::string_view trimmed = input;

    // Trim trailing spaces from the input
    while (!trimmed.empty() && std::isspace(trimmed.back()))
    {
        trimmed.remove_suffix(1);
    }

    // Find the position of the last space
    size_t last_space = trimmed.find_last_of(' ');

    // Return the substring after the last space
    if (last_space == std::string_view::npos)
    {
        return trimmed; // No spaces found, return the whole trimmed string
    }
    else
    {
        return trimmed.substr(last_space + 1);
    }
}

size_t Prompt::countCommonPrefixLength(const std::set<std::string_view> &stringSet) const
{
    if (stringSet.empty())
    {
        return 0; // No strings, no common prefix
    }

    // Take the first string as a reference to compare others
    std::string_view firstString = *stringSet.begin();

    // Initialize the common prefix length to the length of the first string
    size_t commonLength = firstString.length();

    // Iterate through the set to compare each string
    for (const auto &str : stringSet)
    {
        // Find the common prefix length between the current string and the reference
        size_t currentCommonLength = 0;
        for (size_t i = 0; i < std::min(commonLength, str.length()); ++i)
        {
            if (firstString[i] == str[i])
            {
                ++currentCommonLength;
            }
            else
            {
                break;
            }
        }

        // Update the common length
        commonLength = currentCommonLength;

        // Early exit if no common prefix remains
        if (commonLength == 0)
        {
            break;
        }
    }

    return commonLength;
}

std::string_view Prompt::getNwords(const std::string &substr, std::string_view str) const
{
    size_t index;
    for (index = substr.size(); index <= str.size(); index++)
    {
        if (str[index] == ' ')
        {
            break;
        }
    }

    // In case this is the very last word do one step back
    if (index + 1 > str.size())
        index--;

    std::string_view newstr(str.data(), index + 1);
    return newstr;
}

int Prompt::tryMatch(void)
{
    int match_count = 0;
    std::set<std::string_view> matches;

    // Make auxiliary list of strings that matches the input,
    // and will be displayed after pressing TAB
    for (auto &element : m_AuxMenu)
    {
        if (element.first.find(m_Input) == 0)
        {
            match_count++;
            std::string_view Nwords = getNwords(m_Input, element.first);
            matches.emplace(Nwords);
        }
    }
    if (match_count)
        printf("\n");

    for (auto &element : matches)
    {
        std::cout << "\t" << getLastWord((element)) << std::endl;
    }

    // There is only one matching candidate, so fill all other characters
    if (matches.size() == 1)
    {
        std::string_view sv = *(matches.begin());
        m_Input = std::string(sv.substr(0, sv.size() - 1)); // skip passing \n to m_Input otherwise everything after will not be printed

        if (m_Input.back() != ' ')
            m_Input = m_Input + " ";
        return match_count;
    }

    std::string_view refStr = *(matches.begin());

    // Determine if next chars are all the same so we could complete them
    auto num = countCommonPrefixLength(matches);

    if (num)
        m_Input = refStr.substr(0, num); // dummy line

    return match_count;
}

void Prompt::print(void) noexcept
{
    // Print only when content is changed, otherwise skip.
    // Skip printing while entering special chars like \x1b
    if (m_SpecialCharacterState == true || m_oldInput == m_Input)
        return;

#if DEBUG
#ifdef UNIX
    if (m_Prefix.empty() == true)
        printf("\r%s[%s][%u]%s > %s", CYAN_COLOR, m_Name.c_str(), alloc_count, DEFAULT_COLOR, m_Input.c_str());
    else
        printf("\r%s[%s][%u] %s/%s%s > %s", CYAN_COLOR, m_Name.c_str(), alloc_count, GREEN_COLOR, m_Prefix.c_str(), DEFAULT_COLOR, m_Input.c_str());
#else
    if (m_Prefix.empty() == true)
        printf("\r[%s][%u] > %s", m_Name.c_str(), alloc_count, m_Input.c_str());
    else
        printf("\r[%s][%u] %s / > %s", m_Name.c_str(), alloc_count, m_Prefix.c_str(), m_Input.c_str());
#endif
#else
#ifdef UNIX
    if (m_Prefix.empty() == true)
        printf("\r%s[%s]%s > %s", CYAN_COLOR, m_Name.c_str(), DEFAULT_COLOR, m_Input.c_str());
    else
        printf("\r%s[%s] %s/%s%s > %s", CYAN_COLOR, m_Name.c_str(), GREEN_COLOR, m_Prefix.c_str(), DEFAULT_COLOR, m_Input.c_str());
#else
    if (m_Prefix.empty() == true)
        printf("\r[%s] > %s", m_Name.c_str(), m_Input.c_str());
    else
        printf("\r[%s] %s / > %s", m_Name.c_str(), m_Prefix.c_str(), m_Input.c_str());
#endif
#endif

    // Store previous content
    m_oldInput = m_Input;
}

void Prompt::attachFnKeyCallback(FnKey key, const std::function<void()> &cb)
{
    if (static_cast<size_t>(key) < m_FnKeyCallback.size())
        m_FnKeyCallback[static_cast<int>(key)] = cb;
    else
        throw std::out_of_range("Invalid index for FnKey");
}

// Generate auxiliary menu from given starting point
void Prompt::updateAuxMenu(const std::string &prefix)
{
    m_AuxMenu.clear();
    m_CommandHistory.clear();
    m_HistoryIndex = -1;

    if (prefix.size() == 0)
    {
        for (auto &element : m_MainMenu)
        {
            m_AuxMenu[element.first] = element.second;
        }
        return;
    }

    for (auto &element : m_MainMenu)
    {
        if (element.first.find(prefix) == 0)
        {
            std::string_view newstr(element.first.data() + prefix.size(), element.first.size() - prefix.size());
            if (newstr[0] == ' ')
                newstr.remove_prefix(1);

            m_AuxMenu.insert({newstr, element.second});
        }
    }

    m_Prefix = prefix;

    if (m_Prefix[0] == ' ')
        m_Prefix.erase(m_Prefix.begin());
}

void Prompt::insertMenuItem(std::string &&str, Callback cb)
{
    m_MainMenu.emplace(std::move(str), cb);
}
