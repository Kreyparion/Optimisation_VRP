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

    // Lecture de distance.csv
    std::ifstream distanceFile("../tab1/distance.csv");
    // Ignorer la première ligne contenant les en-têtes
    std::getline(distanceFile, line); // Cette commande ignore la première ligne

    int rowNumber = 0; // Utilisé pour compter le nombre de lignes de données
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
    // Le nombre de sommets est égal au nombre de lignes de données
    config.nbVertex = rowNumber; 


    //start reading vehicule to have: 
    // nbVehicle;
    // nbShortTermVehicle;
    // speed;
    // fixedCostShortTermVehicle;
    // fixedCostVehicle;
    // timePenalty;
    // distancePenalty;
    // HardTimeLimit;
    // SoftTimeLimit;
    // SoftDistanceLimit;
    // HardDistanceLimitShortTermVehicle;
    // Capacity;
    // Demand;

    std::ifstream vehicleFile("../tab1/vehicle.csv");
    // Ignorer la première ligne contenant les en-têtes de colonnes
    std::getline(vehicleFile, line); // Ignorer les noms des véhicules

    while (std::getline(vehicleFile, line)) {
        std::stringstream linestream(line);
        std::string parameterName;
        std::getline(linestream, parameterName, '\t'); //récupère le prochain string séparé par \t
        
        std::vector<float> values; // Pour stocker les valeurs numériques des paramètres
        std::string value;
        while (std::getline(linestream, value, '\t')) {
            if (value != "-") { // Ignorer les valeurs manquantes
                try {
                    values.push_back(std::stof(value)); // Convertir en float et stocker
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing value: " << e.what() << '\n';
                }
            } else {
                values.push_back(-1); // Utiliser -1 pour représenter une valeur manquante
            }
        }

        // Attribuer les valeurs lues aux attributs appropriés de config
        if (parameterName == "Number of Vehicles") {
            config.nbVehicle = static_cast<int>(values[0]); // Exemple si vous comptez tous les véhicules comme un total
            config.nbShortTermVehicle = static_cast<int>(values[4] + values[5]); // Exemple pour les véhicules à court terme
        } else if (parameterName == "Capacity") {
            config.Capacity = values; // Assurez-vous que Capacity est un std::vector<float>
        } else if (parameterName == "Average Speed") {
            config.speed = values;
        } else if (parameterName == "Fixed Cost") {
            config.fixedCostVehicle = {values[0], values[1], values[2]}; // Pour les véhicules à long terme
            config.fixedCostShortTermVehicle = {values[3], values[4]}; // Pour les véhicules à court terme
        } else if (parameterName == "Soft Time Limit" || parameterName == "Hard Time Limit") {
            // Assurez-vous d'avoir des attributs correspondants dans Config pour stocker ces valeurs
            // Exemple :
            if (parameterName == "Soft Time Limit") {
                config.SoftTimeLimit = values;
            } else {
                config.HardTimeLimit = values;
            }
        } // Continuez avec les autres paramètres de manière similaire

        // Répétez pour les autres paramètres selon le schéma ci-dessus
    }

    vehicleFile.close();

    std::cout << config;

    return config;
}
