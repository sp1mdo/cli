#include <cstring>
#include <sstream>
#include <set>
#include "Prompt.hpp"

#define GREEN_COLOR "\033[32m"
#define CYAN_COLOR "\033[36m"
#define DEFAULT_COLOR "\033[0m"

const std::string up_key = "\x1b\x5b\x41";
const std::string down_key = "\x1b\x5b\x42";
const std::string left_key = "\x1b\x5b\x44";
const std::string right_key = "\x1b\x5b\x43";

#ifdef UNIX
const char newline_char = 10;
const char backspace_char = 0x7f;
#elif PICO_ON_DEVICE // for example VT102 serial terminal
#warning "this is pico project"
const char newline_char = 13;
const char backspace_char = 0x08;
#endif
const char tab_char = 0x09;

void Prompt::setNonCanonicalMode(void)
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

void Prompt::run(void)
{
    updateAuxMenu("");
    print();
    while (1)
    {
        char z = getc(stdin);
        {
            bool special_handling = false;
            // printf("\r%02x\n", z); // debug purpose
            // continue;

            if ((z == backspace_char && m_Input.empty() == true && m_Prefix.empty() == false)) // esc - todo hadle ESC in handleSpecialCharacters
            {
                removeLastWord(m_Prefix);
                updateAuxMenu(m_Prefix);
                //m_Prefix.clear();
                m_Input.clear();
                clear_line(20);
                print();
                continue;
            }

            if (z != tab_char) // any valid char, but not tab
            {
                m_Input.push_back(z);
                special_handling= handleSpecialCharacters();
            }
            size_t num;
            if (z == backspace_char) // backspace
            {
                backspace();
            }
            else if (z == tab_char) // tab
            {
                num = try_match();
            }

            if (z != tab_char || num > 0 || special_handling)
            {
                print();
            }

            if (z == newline_char) // newline //10 in Linux
            {
                parseCommand();
                print();
            }
        }
    }
}

void Prompt::debug(void)
{
    printf("\n[%s] \n", m_Input.c_str());
}

void dumpString(const std::string &str)
{
    printf("\n[");
    for (auto &xchar : str)
    {
        printf("%02x ", xchar);
    }
    printf("]\n");
}

void Prompt::removeLastWord(std::string &str)
{
    while(str.empty() == false)
    {
        if(str.back() == ' ')
            break;

        str.pop_back();
    }
    str.pop_back();
}

bool Prompt::handleSpecialCharacters(void)
{
    // dumpString(m_Input);
    if (m_Input.find(up_key) != std::string::npos)
    {
        if (m_CommandHistory.empty() == true)
        {
            m_Input.clear();
            //printf("\n");
            return true;
        }
        m_HistoryIndex++;
        if (m_HistoryIndex >= 0)
        {
            if (m_HistoryIndex >= (int)m_CommandHistory.size())
                m_HistoryIndex = m_CommandHistory.size() - 1;
        }

        m_Input = m_CommandHistory[m_HistoryIndex];

        printf("\n");
        clear_line(50);
        return true;
    }
    if (m_Input.find(down_key) != std::string::npos)
    {
        clear_line(50);
        if (m_CommandHistory.empty() == true)
        {
            m_Input.clear();
            clear_line(50);
            return true;
        }

        m_HistoryIndex--;
        if (m_HistoryIndex < 0)
        {
            m_Input.clear();
            clear_line(50);
            m_HistoryIndex = -1;
        }
        else
            m_Input = m_CommandHistory[m_HistoryIndex];

        
        return true;
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

    clear_line(20);

    return true;
}

void Prompt::parseCommand(void)
{
    while (m_Input.back() == ' ' || m_Input.back() == newline_char) // trim all newline chars and spaces at the end of input str
        m_Input.pop_back();

    size_t cnt = 0;
    bool last = false;
    bool found = false;

    // Check whether given input is found in command list
    for (auto &element : m_AuxMenu)
    {
        // Check whether this string is the full word
        if (element.first.find(m_Input + " ") != std::string::npos)
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
            fprintf(stderr, "Unknown command\n");

        m_Input.clear();
        clear_line(20);
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
            m_AuxMenu.at(command)(args);
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

            if (m_CommandHistory.size() > 20) // Limit the command history size
                m_CommandHistory.erase(m_CommandHistory.end());

            m_HistoryIndex = -1;
            break;
        }
    }

    if (cnt == 0 && (executed == false))
    {
        fprintf(stderr, "Unknown command\n");
    }

    m_Input.clear();
    clear_line(50);
}

void Prompt::clear_line(size_t chars)
{
    for (size_t i = 0; i < chars; i++)
        printf("\b");
    for (size_t i = 0; i < chars; i++)
        printf(" ");
    for (size_t i = 0; i < chars; i++)
        printf("\b");
}

std::string Prompt::getFirstNWords(const std::string &input, size_t N)
{
    std::istringstream stream(input);
    std::string word;
    std::vector<std::string> words;

    // Extract words from the string stream
    while (stream >> word && words.size() < N)
    {
        words.emplace_back(std::move(word));
    }

    // Combine the first N words back into a single string
    std::ostringstream result;
    for (size_t i = 0; i < words.size(); ++i)
    {
        if (i > 0)
            result << " "; // Add a space between words
        result << std::move(words[i]);
    }

    return result.str();
}

size_t Prompt::countCharacterOccurrences(const std::string &input, char target)
{
    return std::count(input.begin(), input.end(), target);
}

std::string Prompt::getLastWord(const std::string &input)
{
    // Trim trailing whitespace (if any)
    auto end = std::find_if(input.rbegin(), input.rend(), [](char c)
                            { return !std::isspace(static_cast<unsigned char>(c)); })
                   .base();

    if (end == input.begin())
    {
        return ""; // Return empty string if input is all whitespace
    }

    // Find the beginning of the last word
    auto start = std::find_if(std::string::const_reverse_iterator(end), input.rend(), [](char c)
                              { return std::isspace(static_cast<unsigned char>(c)); })
                     .base();

    return std::string(start, end);
}

// Function to determine the number of identical first characters shared by all strings in the set
size_t Prompt::countCommonPrefixLength(const std::set<std::string> &stringSet)
{
    if (stringSet.empty())
    {
        return 0; // No strings, no common prefix
    }

    // Take the first string as a reference to compare others
    const std::string &firstString = *stringSet.begin();

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

int Prompt::try_match(void)
{
    int match_count = 0;
    std::vector<int> matching_indexes;
    std::set<std::string> matches;

    for (auto &element : m_AuxMenu)
    {
        if (element.first.find(m_Input) == 0)
        {
            // chars_matching = m_Input.size();
            match_count++;
            matches.emplace(getFirstNWords(element.first, countCharacterOccurrences(m_Input, ' ') + 1) + " ");
        }
    }
    if (match_count)
        printf("\n");

    for (auto &element : matches)
    {
        printf("\t%s \n", getLastWord(element).c_str());
    }

    if (matches.size() == 1)
    {
        m_Input = *(matches.begin());
        return match_count;
    }
    const std::string &refStr = *(matches.begin());

    // Determine if next chars are all the same so we could complete them
    auto num = countCommonPrefixLength(matches);

    if (num)
        m_Input = refStr.substr(0, num); // dummy line

    return match_count;
}

void Prompt::print(void)
{
    if (m_Prefix.empty() == true)
        printf("\r%s[%s][%d]%s > %s", CYAN_COLOR, m_Name.c_str(), m_HistoryIndex, DEFAULT_COLOR, m_Input.c_str());
    else
        printf("\r%s[%s] %s/%s%s > %s", CYAN_COLOR, m_Name.c_str(), GREEN_COLOR, m_Prefix.c_str(), DEFAULT_COLOR, m_Input.c_str());
}

// Generate auxiliary menu from given starting point
void Prompt::updateAuxMenu(const std::string &prefix)
{
    m_AuxMenu.clear();
    m_CommandHistory.clear();
    m_HistoryIndex = -1;
    for (auto &element : m_MainMenu)
    {
        if (element.first.find(prefix) == 0)
        {
            std::string newstr(element.first, prefix.size(), element.first.size());
            if (newstr[0] == ' ')
                newstr.erase(newstr.begin());

            m_AuxMenu.insert({newstr, element.second});
        }
    }

    m_Prefix = prefix;

    if (m_Prefix[0] == ' ')
        m_Prefix.erase(m_Prefix.begin());

    if (m_AuxMenu.size() == 0)
    {
        m_AuxMenu = m_MainMenu;
        m_Prefix.clear();
    }
}

void Prompt::insertMapElement(std::string &&str, Callback cb)
{
    m_MainMenu.emplace(std::move(str), cb);
}

void Prompt::setSpecialCharsHandling(bool flag)
{
    m_SpecialCharsHandling = flag;
}

bool Prompt::isSpecialCharsHandlingEnabled(void)
{
    return m_SpecialCharsHandling;
}
