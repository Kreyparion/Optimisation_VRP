#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <config.h>

Config import_data() {
    Config config;

    // Exemple de lecture d'un fichier CSV pour les demandes
    std::ifstream demandFile("tab1/demand.csv");
    std::string line;
    while (std::getline(demandFile, line)) { //sert à parser 
        std::stringstream linestream(line);
        std::string value;
        while (std::getline(linestream, value, '\t')) { // Assumons une séparation par virgule --> getline sert à parser
            config.Demand.push_back(std::stoi(value)); //string to int car on veut vector de int
        }
    }

    std::ifstream distanceFile("tab1/distance.csv");
    std::string line;
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

    // Assurez-vous de fermer les fichiers après les avoir lus
    demandFile.close();

    return config;
}
