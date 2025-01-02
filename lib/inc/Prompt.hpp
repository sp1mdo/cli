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

template <typename K, typename V>
class Flat_Map {
private:
    std::vector<std::pair<K, V>> data;

public:
    // Insert a key-value pair (insert behaves like std::map::insert)
    std::pair<typename std::vector<std::pair<K, V>>::iterator, bool> insert(const std::pair<K, V>& pair) {
        auto it = std::lower_bound(data.begin(), data.end(), pair, 
            [](const std::pair<K, V>& a, const std::pair<K, V>& b) {
                return a.first < b.first;
            });

        if (it != data.end() && it->first == pair.first) {
            // Key already exists, do not insert, return iterator and false
            return {it, false};
        }

        // Key does not exist, insert the new pair
        return {data.insert(it, pair), true};
    }

    // Insert using move semantics (emulates std::map::emplace)
    template <typename... Args>
    std::pair<typename std::vector<std::pair<K, V>>::iterator, bool> emplace(Args&&... args) {
        std::pair<K, V> pair(std::forward<Args>(args)...);

        // Avoid inserting if the key already exists
        return insert(pair);
    }

    // Find an element by key
    typename std::vector<std::pair<K, V>>::iterator find(const K& key) {
        auto it = std::lower_bound(data.begin(), data.end(), key, 
            [](const std::pair<K, V>& a, const K& b) {
                return a.first < b;
            });

        if (it != data.end() && it->first == key) {
            return it;
        }
        return data.end();
    }

    // Access value by key using operator[]
    V& operator[](const K& key) {
        auto it = std::lower_bound(data.begin(), data.end(), key, 
            [](const std::pair<K, V>& a, const K& b) {
                return a.first < b;
            });

        if (it != data.end() && it->first == key) {
            return it->second; // Key found, return reference to value
        }

        // If key doesn't exist, insert a new element with default value
        data.insert(it, {key, V()});
        return it->second;
    }

    // Access value by key, throws std::out_of_range if key not found
    V& at(const K& key) {
        auto it = std::lower_bound(data.begin(), data.end(), key, 
            [](const std::pair<K, V>& a, const K& b) {
                return a.first < b;
            });

        if (it == data.end() || it->first != key) {
            //throw std::out_of_range("Key not found in FlatMap");
        }

        return it->second; // Return reference to the value
    }

    void reserve(size_t num)
    {
        data.reserve(num);
    }

    // Begin iterator
    typename std::vector<std::pair<K, V>>::iterator begin() {
        return data.begin();
    }

    // End iterator
    typename std::vector<std::pair<K, V>>::iterator end() {
        return data.end();
    }

    // Const begin iterator
    typename std::vector<std::pair<K, V>>::const_iterator begin() const {
        return data.begin();
    }

    // Const end iterator
    typename std::vector<std::pair<K, V>>::const_iterator end() const {
        return data.end();
    }

    // Clear all elements
    void clear() {
        data.clear();
    }

    // Size of the flat_map
    size_t size() const {
        return data.size();
    }

    // Check if the map is empty
    bool empty() const {
        return data.empty();
    }
};

class Prompt
{
public:
    Prompt(const std::string &name) : m_Name(name), m_HistoryIndex(-1), m_SpecialCharsHandling(true)
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
    void setNonCanonicalMode(void);
    void updateAuxMenu(const std::string &prefix);
    void setSpecialCharsHandling(bool flag);
    bool isSpecialCharsHandlingEnabled(void);

    //std::map<std::string, Callback> m_MainMenu;
    //std::map<std::string, Callback> m_AuxMenu;

    Flat_Map<std::string, Callback> m_MainMenu;
    Flat_Map<std::string, Callback> m_AuxMenu;

private:
    std::string m_Input;
    std::string m_Prefix;
    std::string m_Name;
    Tokens m_CommandHistory;
    int m_HistoryIndex;
    bool m_SpecialCharsHandling;
    bool handleSpecialCharacters(void);
    void removeLastWord(std::string &str);
    size_t countCommonPrefixLength(const std::vector<std::string> &stringSet);
    size_t countCommonPrefixLength(const std::set<std::string> &stringSet);
    size_t countCharacterOccurrences(const std::string &input, char target);
    std::string getLastWord(const std::string &input);
    void clear_line(size_t chars);
    void debug(void);
    std::string getFirstNWords(const std::string &input, size_t N);
};
