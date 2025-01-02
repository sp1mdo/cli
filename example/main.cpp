#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <fstream>
#include <set>
#include <cstring>
#include <map>

#include "../lib/inc/Prompt.hpp"

#define GREEN_COLOR "\033[32m"
#define DEFAULT_COLOR "\033[0m"

void callback(int id, const std::string &str)
{
    printf("Received : id=%d arg=[%s] \n ", id, str.c_str());
}

int main(int argc, char **argv)
{
    Prompt my_prompt("AHU_2040");

    /*
    std::ifstream file(argv[1]);
    // Check if the file was successfully opened
    
    if (!file.is_open())
    {
        fprintf(stderr, "Failed to open the file.");
        return 1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::transform(line.begin(), line.end(), line.begin(),  [](unsigned char c){ return std::tolower(c); });
        my_prompt.insertMapElement(line, callback);
    }
    */
    int fun_id = 0;

    my_prompt.m_MainMenu.reserve(50);
    my_prompt.insertMapElement("settings show", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("settings save", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("settings read", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("settings restore_default", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("system bootsel", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("system reset", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("system show info", std::bind(callback, 0, std::placeholders::_1));

    my_prompt.insertMapElement("operation show info", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("operation set idle", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("operation set cool", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("operation set heat", std::bind(callback, 0, std::placeholders::_1));

    my_prompt.insertMapElement("modbus show input_registers", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("modbus show holding_registers", std::bind(callback, 1, std::placeholders::_1));
    my_prompt.insertMapElement("modbus set register", std::bind(callback, 2, std::placeholders::_1));
    my_prompt.insertMapElement("modbus show settings", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("modbus set debug toggle", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("control set local_0-10V", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("control set remote_0-100", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("control set remote_temperature", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("power set level", std::bind(callback, 0, std::placeholders::_1));

    my_prompt.insertMapElement("power increment", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("power decrement", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature set target", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature set mode static", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature set mode dynamic", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature set delta_low", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature set delta_high", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature set idle_time", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature test dynamic", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature show target", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature pid set k_p", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature pid set k_i", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("temperature pid set k_d", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("oil set low_freq", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("oil set interval", std::bind(callback, 0, std::placeholders::_1));
    my_prompt.insertMapElement("oil set target_frequency", std::bind(callback, 0, std::placeholders::_1));

#ifdef DEVELOPER
    my_prompt.insertMapElement("expert enable", std::bind(callback, fun_id++, std::placeholders::_1));
    my_prompt.insertMapElement("expert disable", std::bind(callback, fun_id++, std::placeholders::_1));
    my_prompt.insertMapElement("expert set compressor frequency", std::bind(callback, fun_id++, std::placeholders::_1));
    my_prompt.insertMapElement("expert set fan frequency", std::bind(callback, fun_id++, std::placeholders::_1));
    my_prompt.insertMapElement("expert set eev position", std::bind(callback, fun_id++, std::placeholders::_1));
#endif

    my_prompt.updateAuxMenu("");

    my_prompt.run();

    return 0;
}