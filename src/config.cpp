#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include "config.h"


Config import_data(int num, bool verbose=0) {
    Config config;

    std::ostringstream path_builder;
    path_builder << "../tables/tab" << num << "/demand.csv";
    std::string file_path = path_builder.str();

    std::ifstream file(file_path);
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
    std::ostringstream path_builder2;
    path_builder2 << "../tables/tab" << num << "/distance.csv";
    std::string file_path2 = path_builder2.str();
    std::ifstream file2(file_path2);
    // Ignorer la première ligne contenant les en-têtes
    std::getline(file2, line); // Cette commande ignore la première ligne

    int rowNumber = 0; // Utilisé pour compter le nombre de lignes de données
    while (std::getline(file2, line)) {
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

    //start reading vehicule

    std::ostringstream path_builder3;
    path_builder3 << "../tables/tab" << num << "/vehicule_cleaned.csv";
    std::string file_path3 = path_builder3.str();
    std::ifstream file3(file_path3);
    // Ignorer la première ligne contenant les en-têtes de colonnes
    std::getline(file3, line); // Ignorer les noms des véhicules

    while (std::getline(file3, line)) {
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
            
            config.vehicleCounts = {values[0], values[1], values[2]};
            config.shortTermVehicleCounts = {values[3], values[4]};
            for (auto elem:{values[0], values[1], values[2]}) {
                config.nbVehicle += elem;
            } 
            for (auto elem:{values[3], values[4]}) {
                config.nbShortTermVehicle += elem;
            }
        } else if (parameterName == "Capacity") {
            config.Capacity = {values[0], values[1], values[2]}; //Capacity est un std::vector<float>
        } else if (parameterName == "Average Speed") {
            config.speed = {values[0], values[1], values[2]};
        } else if (parameterName == "Fixed Cost") {
            config.fixedCostVehicle = {values[0], values[1], values[2]}; // Pour les véhicules à long terme
            config.fixedCostShortTermVehicle = {values[3], values[4]}; // Pour les véhicules à court terme
        } else if (parameterName == "Soft Time Limit" || parameterName == "Hard Time Limit") {
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
    }

    file3.close();

    if(verbose>1.0){
        std::cout << config;
    }

    return config;
}

void extend_config(Config& config) {
    // For Long Term Vehicles
    for(int i=0; i<3; i++){
        for(int k=0; k<config.vehicleCounts[i]; k++){
            config.fixedCostVehicle.push_back(config.fixedCostVehicle[i]);
            config.speed.push_back(config.speed[i]);
            config.timePenalty.push_back(config.timePenalty[i]);
            config.distancePenalty.push_back(config.distancePenalty[i]);
            config.HardTimeLimit.push_back(config.HardTimeLimit[i]);
            config.SoftTimeLimit.push_back(config.SoftTimeLimit[i]);
            config.SoftDistanceLimit.push_back(config.SoftDistanceLimit[i]);
            config.Capacity.push_back(config.Capacity[i]);
        }
    }
    for(int i=0; i<2; i++){
        for(int k=0; k<config.shortTermVehicleCounts[i]; k++){
            config.fixedCostShortTermVehicle.push_back(config.fixedCostShortTermVehicle[i]);
            config.HardDistanceLimitShortTermVehicle.push_back(config.HardDistanceLimitShortTermVehicle[i]);
        }
    }
}

Config getConfig(int num) {
    Config config = import_data(num);
    extend_config(config);
    return config;
}