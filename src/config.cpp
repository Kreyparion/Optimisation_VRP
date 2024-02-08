#include <fstream>
#include <sstream>
#include <vector>
#include <string>

Config import_data() {
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
