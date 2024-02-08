// Import of the data
#pragma once
#include "config.h"
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <memory>
#include <iostream>

Config import_data(){
    Config config;

    std::ifstream file("../tab1/demand.csv");
    std::string line;
    while (std::getline(file, line)){
        std::string token;
        std::istringstream tokenStream(line);
        std::vector<std::string> tokens;
        std::getline(tokenStream, token, '\t');
        std::getline(tokenStream, token, '\t');
        config.Demand.push_back(std::stoi(token));
    }
    file.close();
    std::cout << config;
    return config;
}