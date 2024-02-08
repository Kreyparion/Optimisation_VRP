#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include "config.h"

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

    std::ifstream distanceFile("../tab1/distance.csv");
    // Ignorer la première ligne si elle contient des en-têtes de colonnes
    // std::getline(distanceFile, line); // Décommentez si la première ligne est un en-tête
    int rowNumber = 0; // Compteur pour suivre le nombre de lignes (et donc de sommets)
    while (std::getline(distanceFile, line)) {
        std::stringstream linestream(line);
        std::string value;
        while (std::getline(linestream, value, '\t')) { // Séparation par tabulation
            try {
                // Convertir la valeur string en int et l'ajouter au vecteur
                config.dist.push_back(std::stoi(value)); // string to int
            } catch (const std::invalid_argument& ia) {
                std::cerr << "Invalid argument: " << ia.what() << '\n';
            } catch (const std::out_of_range& oor) {
                std::cerr << "Out of Range error: " << oor.what() << '\n';
            }
        }
        ++rowNumber; // Incrémenter le compteur de lignes après avoir traité une ligne
    }
    config.nbVertex = rowNumber; // Mettre à jour le nombre de sommets dans config

    
    std::cout << config;
    return config;
}
