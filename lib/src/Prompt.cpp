#include <cstring>
#include <sstream>
#include <set>
#include "Prompt.hpp"

#define GREEN_COLOR "\033[32m"
#define DEFAULT_COLOR "\033[0m"

void Prompt::setNonCanonicalMode(void)
{
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
}

void Prompt::run(void)
{
    while (1)
    {
        print();
        char z = getc(stdin);
        {
            // printf("\r%02x\n", z); // debug purpose
            // continue;

            if (z == 0x1b) // esc
            {
                m_Input.clear();
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

bool Prompt::backspace(void)
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
    printf("parse");
    while (m_Input.back() == ' ' || m_Input.back() == 0x0a) // trim all newline chars and spaces at the end of input str
        m_Input.pop_back();

    for (size_t i = 0; i < m_Input.size(); i++)
    {
        std::string command(m_Input, 0, i + 1); //find a moment where the command is separated from the args
        if (m_MapMenu.find(command) != m_MapMenu.end())
        {
            std::string args(m_Input, i + 1, sizeof(m_Input));
            while (args.front() == ' ')
                args.erase(args.begin());

            m_MapMenu.at(command)(args); //execute callabck with given args
            break;
        }
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

size_t countCharacterOccurrences(const std::string &input, char target)
{
    return std::count(input.begin(), input.end(), target);
}

std::string getLastWord(const std::string &input)
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

    for (auto &element : m_MapMenu)
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
    printf("\n");

    return match_count;
}

void Prompt::print(void)
{
    printf("\r# %s", m_Input.c_str());
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

void Prompt::insertMapElement(std::string &&str, Callback cb)
{
    m_MapMenu.insert({str, cb});
}
