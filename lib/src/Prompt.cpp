#include <cstring>
#include <sstream>
#include "Prompt.hpp"

#define GREEN_COLOR "\033[32m"
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
    while (1)
    {
        print();
        #ifdef UNIX
        char z = getc(stdin);
        #elif WIN32
        int z = _getch();
        #endif
        {
            // printf("\r%02x\n", z); // debug purpose
            // continue;

            if (z == 0x1b) // esc
            {
                goToRoot();
                continue;
            }
            if (z != 9 && z != 0x7f) // any valid char, but not tab
            {
                push_back(z);
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
    printf("\nx[(%zu)%s][%s] \n", m_Prefix.size(), printTokens(m_Prefix).c_str(), m_Input.c_str());
}

void Prompt::push_back(char c)
{
    Menu* old_menu = this->m_CurrentMenu;
    m_Input.push_back(c);
    organize(c);

    if (c == ' ' && old_menu == this->m_CurrentMenu)
    {
        m_Input.push_back(c);
    }
}

std::string *Prompt::getInput(void)
{
    return &m_Input;
}

bool Prompt::backspace(void)
{
    printf("\b \b");

    if (m_Input.size() > 0)
    {
        m_Input.pop_back();
        if (m_Prefix.empty() == true)
            return false;
    }
    else if (m_Prefix.size() > 0)
    {
        if (m_Prefix.empty() == false)
        {
            while (m_Prefix.back().size() == 0 && m_Prefix.size() > 0)
            {
                m_Prefix.pop_back();
            }
        }
        if (m_Prefix.empty() == false)
            m_Input = std::move(m_Prefix.back());

        if (m_CurrentMenu->getParent() != nullptr)
            m_CurrentMenu = m_CurrentMenu->getParent();

        if (m_Input.size() > 0)
        {
            m_Input.pop_back();
        }
    }

    if (m_Prefix.size() > 0)
    {
        while (m_Prefix.back().empty() == true)
        {
            m_Prefix.pop_back();
            break;
        }
    }

    clear_line(20);

    return true;
}

void Prompt::parseCommand(void)
{
    if (m_Input.back() == '\n')
        m_Input.pop_back();

    // printf("entered command : %s [%s] \n", tokensToString(m_Prefix, true).c_str(),m_Input.c_str());

    while (m_CurrentMenu->getParent() != nullptr)
        m_CurrentMenu = m_CurrentMenu->getParent(); // Finding the root

    for (auto &token : m_Prefix)
    {
        if (m_CurrentMenu->getElement(token) != nullptr && m_CurrentMenu->getElement(token)->getSubMenu() != nullptr)
        {
            m_CurrentMenu = m_CurrentMenu->getElement(token)->getSubMenu();
        }
        else
        {
            m_CurrentMenu->getElement(token)->Function(tokenize(m_Input));
            break;
        }
    }

    printf("\n");
    m_Prefix.clear();
    m_Input.clear();

    while (m_CurrentMenu->getParent() != nullptr)
        m_CurrentMenu = m_CurrentMenu->getParent();
}

void Prompt::goToRoot(void)
{
    while (m_CurrentMenu->getParent() != nullptr)
        m_CurrentMenu = m_CurrentMenu->getParent();

    clear_line(20);
    m_Input.clear();
    m_Prefix.clear();
}

void Prompt::init(Menu *menu)
{
    m_CurrentMenu = menu;

    setNonCanonicalMode();
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

int Prompt::try_match(void)
{
    int match_count = 0;
    std::vector<int> matching_indexes;
    size_t index;
    size_t chars_matching;
    if (m_Prefix.empty() == false)
    {
        if (m_Input.empty() == true && m_CurrentMenu->getElement(m_Prefix.back()) != nullptr)
            return 0;
    }

    for (size_t i = 0; i < m_CurrentMenu->m_Entities.size(); i++)
    {
        if (m_CurrentMenu->m_Entities[i].getLabel().find(m_Input) == 0)
        {
            matching_indexes.push_back(i);
            index = i;
            match_count++;
            chars_matching = m_Input.size();
        }
    }

    if (matching_indexes.size() > 0)
    {
        printf("\n");
        for (size_t i = 0; i < matching_indexes.size(); i++)
        {
            printf("\t%s\n", m_CurrentMenu->m_Entities[matching_indexes[i]].getLabel().c_str());
        }
    }

    if (match_count >= 1)
    {
        m_Input.clear();
        if (match_count == 1)
            m_Input = m_CurrentMenu->m_Entities[index].getLabel();
        else
            m_Input.assign(m_CurrentMenu->m_Entities[index].getLabel(), 0, chars_matching);

        if (match_count == 1)
            organize(' ');
    }

    return match_count;
}
void Prompt::organize(char c)
{
    auto inputTokens = tokenize(m_Input);
    if(inputTokens.empty() == true)
        return;
    
    m_Input = tokensToString(inputTokens, true);
    if (c == ' ' && m_CurrentMenu->getElement(inputTokens[0]) != nullptr)
    {
        m_Prefix.push_back(m_Input);
        m_Input.clear();
        if (m_CurrentMenu->getElement(m_Prefix.back()) != nullptr && m_CurrentMenu->getElement(m_Prefix.back())->getSubMenu() != nullptr)
        {
            printf("going to (%s)\n", m_Prefix.back().c_str());
            m_CurrentMenu = m_CurrentMenu->getElement(m_Prefix.back())->getSubMenu();
        }
    }
}

void Prompt::print(void)
{
    std::string prefixStr = tokensToString(m_Prefix, true);

    if (m_Prefix.empty())
        printf("\r# %s", m_Input.c_str());
    else
        printf("\r# %s%s%s %s", GREEN_COLOR, prefixStr.c_str(), DEFAULT_COLOR, m_Input.c_str());
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
    if(str == " ")
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

/*
std::vector<std::string> Prompt::tokenize(const std::string &input)
{
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string word;

    // Extract words separated by whitespace
    while (stream >> word)
    {
        tokens.push_back(word);
    }

    return tokens;
}*/