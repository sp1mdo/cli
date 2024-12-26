#include <cstring>
#include <sstream>
#include <set>
#include "Prompt.hpp"

#define GREEN_COLOR "\033[32m"
#define CYAN_COLOR "\033[36m"
#define DEFAULT_COLOR "\033[0m"

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
    while (1)
    {
        print();
        char z = getc(stdin);
        {
            // printf("\r%02x\n", z); // debug purpose
            // continue;

            if (z == 0x1b || (z == 0x7f && m_Input.empty() == true)) // esc
            {
                updateAuxMenu("");
                m_Prefix.clear();
                m_Input.clear();
                clear_line(20);
                continue;
            }
            if (z != 9 && z != 0x7f) // any valid char, but not tab
            {
                m_Input.push_back(z);
                print();
            }

            if (z == 0x7f) // backspace
            {
                backspace();
                continue;
            }
            else if (z == 9) // tab
            {
                try_match();
            }

            // handle_special_chars(index, input);
            if (z == 10) // newline //10 in Linux
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

bool Prompt::backspace(void) // todo void
{
    printf("\b \b");

    if (m_Input.size() > 0)
    {
        m_Input.pop_back();
    }

    clear_line(20);

    return true;
}

void Prompt::parseCommand(void)
{
    while (m_Input.back() == ' ' || m_Input.back() == 0x0a) // trim all newline chars and spaces at the end of input str
        m_Input.pop_back();

    size_t cnt = 0;
    bool last = false;
    bool found = false;

    for (auto &element : m_AuxMenu) // check wether given input is found in command list
    {
        if (element.first.find(m_Input + " ") != std::string::npos) // check wether this string is the full word
        {
            found = true;
        }

        if (element.first.find(m_Input) != std::string::npos)
        {
            cnt++; // TODO : make check if the word is not truncated, for example : po instead of poland
            if (getLastWord(element.first) == m_Input && cnt == 1)
                last = true; // don't make prefix if the word is the last one, so it's the command actually
        }
    }

    std::string updatestr;
    if (m_Prefix.empty() == true)
        updatestr = m_Input;
    else
        updatestr = m_Prefix + " " + m_Input;

    if (cnt >= 1 && m_Input.empty() == false && last == false && m_AuxMenu.find(updatestr) == m_AuxMenu.end())
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
        std::string command(m_Input, 0, i + 1); // find a moment where the command is separated from the args
        if (m_AuxMenu.find(command) != m_AuxMenu.end())
        {
            std::string args(m_Input, i + 1, sizeof(m_Input));
            while (args.front() == ' ')
                args.erase(args.begin());

            m_AuxMenu.at(command)(args); // execute callabck with given args
            executed = true;
            break;
        }
    }
   
    if (cnt == 0 && (executed == false || found == false))
    {
        fprintf(stderr, "Unknown command\n");
    }

    m_Input.clear();
    clear_line(20);
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
            matches.emplace(getFirstNWords(element.first, countCharacterOccurrences(m_Input, ' ') + 1) + " ");
        }
    }
    printf("\n");
    for (auto &element : matches)
    {
        printf("\t%s \n", getLastWord(element).c_str());
    }

    if (matches.size() == 1)
        m_Input = *(matches.begin());

    return match_count;
}

void Prompt::print(void)
{
    if (m_Prefix.empty() == true)
        printf("\r%s[%s]%s > %s", CYAN_COLOR, m_Name.c_str(), DEFAULT_COLOR, m_Input.c_str());
    else
        printf("\r%s[%s] %s/%s%s > %s", CYAN_COLOR, m_Name.c_str(), GREEN_COLOR, m_Prefix.c_str(), DEFAULT_COLOR, m_Input.c_str());
}

void Prompt::updateAuxMenu(const std::string &prefix)
{
    m_AuxMenu.clear();
    for (auto &element : m_MainMenu)
    {
        if (element.first.find(prefix) == 0)
        {
            std::string newstr(element.first, prefix.size(), element.first.size());
            if (newstr[0] == ' ')
                newstr.erase(newstr.begin());
            // printf("%s\n",newstr.c_str());
            m_AuxMenu.insert({newstr, element.second});
        }
    }

    m_Prefix = prefix;

    if (m_Prefix[0] == ' ')
        m_Prefix.erase(m_Prefix.begin());

    if (m_AuxMenu.size() == 0)
    {
        printf("oops!\n");
        m_AuxMenu = m_MainMenu;
        m_Prefix.clear();
    }
}

std::string Prompt::tokensToString(Tokens &tokens, bool space)
{
    std::string ret;
    for (auto &token : tokens)
    {
        ret = ret + token;
        if (space)
            ret = ret + ' ';
    }
    if (space)
        ret.pop_back();
    return ret;
}

std::string Prompt::printTokens(const std::vector<std::string> &tokens)
{
    std::string ret;
    for (auto &token : tokens)
    {
        ret = ret + token + ":";
    }
    return ret;
}

std::vector<std::string> Prompt::tokenize(const std::string &str)
{
    std::vector<std::string> tokens;
    if (str == " ")
    {
        return tokens;
    }
    char *token = strtok((char *)str.c_str(), " - ");

    while (token != NULL)
    {
        tokens.push_back(std::string(token));
        token = strtok(NULL, " ");
    }
    return tokens;
}

void Prompt::insertMapElement(const std::string &str, Callback cb)
{
    m_MainMenu.insert({str, cb});
}
