#include <fstream>
#include <sstream>
#include <vector>
#include <string>

Config import_data() {
    Config config;

    // Exemple de lecture d'un fichier CSV pour les demandes
    std::ifstream demandFile("demand.csv");
    std::string line;
    while (std::getline(demandFile, line)) {
        std::stringstream linestream(line);
        std::string value;
        while (std::getline(linestream, value, ',')) { // Assumons une séparation par virgule
            config.Demand.push_back(std::stoi(value));
        }
    }

    // Exemple similaire pour les autres fichiers CSV (distance, coordonnées, véhicule, etc.)
    // Vous devrez adapter cette partie selon la structure de vos fichiers CSV
    // et les données spécifiques que vous souhaitez extraire.

    // Assurez-vous de fermer les fichiers après les avoir lus
    demandFile.close();

    return config;
}
