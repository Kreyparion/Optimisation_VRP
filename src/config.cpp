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
        config.Demand.push_back(std::stof(token));
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
                config.dist.push_back(std::stof(value));
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
    // timePenalty;
    // distancePenalty;
    //SoftDistanceLimit:
    //HardDistanceLimitSTV:

    std::ifstream vehicleFile("../tab1/vehicule_cleaned.csv");
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
                values.push_back(1000000);
            }
        }

        // Attribuer les valeurs lues aux attributs appropriés de config
        if (parameterName == "Number of Vehicles") {
            for (auto elem:{values[0], values[1], values[2]}) {
                   config.nbVehicle += elem;             
            } 
            for (auto elem:{values[3], values[4]}) {
                config.nbShortTermVehicle += elem;
            }
        } else if (parameterName == "Capacity") {
            config.Capacity = {values[0], values[1], values[2]}; // Assurez-vous que Capacity est un std::vector<float>
        } else if (parameterName == "Average Speed") {
            config.speed = {values[0], values[1], values[2]};
        } else if (parameterName == "Fixed Cost") {
            config.fixedCostVehicle = {values[0], values[1], values[2]}; // Pour les véhicules à long terme
            config.fixedCostShortTermVehicle = {values[3], values[4]}; // Pour les véhicules à court terme
        } else if (parameterName == "Soft Time Limit" || parameterName == "Hard Time Limit") {
            // Assurez-vous d'avoir des attributs correspondants dans Config pour stocker ces valeurs
            // Exemple :
            if (parameterName == "Soft Time Limit") {
                config.SoftTimeLimit = {values[0], values[1], values[2]};
            } else if(parameterName == "Hard Time Limit") {
                config.HardTimeLimit = {values[0], values[1], values[2]};
            }
        } else if (parameterName == "Time Penalty Cost") {
            config.timePenalty = {values[0], values[1], values[2]};
        } else if (parameterName == "Distance Penalty Cost") {
            config.distancePenalty = {values[0], values[1], values[2]};
        } else if (parameterName == "Soft Distance Limit") {
            config.SoftDistanceLimit = {values[0], values[1], values[2]};
        } else if (parameterName == "Hard Distance Limit") {
            config.HardDistanceLimitShortTermVehicle = {values[3], values[4]};
        }

        // Répétez pour les autres paramètres selon le schéma ci-dessus
    }

    vehicleFile.close();

    std::cout << config;

    return config;
}
