#include "tabou_search.h"

// Implémentation des fonctions de la classe TabouSearch

// Initialisation des membres de la classe si nécessaire
TabouSearch::TabouSearch(const Config &config) : config(config) {
    TabouSearch::config = config;
    num_iterations = 1000;
    tabou_tenure = 10;
}

/*// Générer et retourner une solution initiale valide
std::vector<std::vector<int>> TabouSearch::generateInitialSolution() {
    std::vector<int> solution(config.nbVertex, 0);
    std::vector<float> vehicleLoad(config.nbVehicle + config.nbShortTermVehicle, 0);
    int currentVehicle = 0;

    // Répartition initiale des demandes sur les véhicules en respectant la capacité
    for (int i = 1; i < config.nbVertex; ++i) { // Commencer par 1 car 0 est généralement le dépôt
        if (vehicleLoad[currentVehicle] + config.Demand[i] > config.Capacity[currentVehicle]) {
            // Si le véhicule actuel ne peut pas gérer la demande supplémentaire, passer au suivant
            ++currentVehicle;
            if (currentVehicle >= vehicleLoad.size()) {
                // Si nous n'avons plus de véhicules disponibles, la solution initiale n'est pas viable
                throw std::runtime_error("Not enough vehicles to cover all demands in initial solution.");
            }
        }
        vehicleLoad[currentVehicle] += config.Demand[i];
        solution[i] = currentVehicle;
    }
    displaySolution(solution);
    return solution;
}*/

std::vector<std::vector<int>> TabouSearch::generateInitialSolution() {
    std::vector<std::vector<int>> solution(config.nbVehicle + config.nbShortTermVehicle);

    // Initialiser les tournées pour les véhicules à long terme
    for (int v = 0; v < config.nbVehicle; ++v) {
        solution[v].push_back(0); // Ajouter le dépôt comme point de départ
    }

    // Assigner les clients aux véhicules à long terme en respectant la capacité
    for (int client = 1; client < config.nbVertex; ++client) {
        bool assigned = false;
        for (int v = 0; v < config.nbVehicle && !assigned; ++v) {
            if (canAccommodate(solution[v], client, v)) {
                solution[v].push_back(client);
                assigned = true;
            }
        }

        // Si aucun véhicule à long terme ne peut prendre le client, essayer les véhicules à court terme
        if (!assigned) {
            for (int v = config.nbVehicle; v < solution.size() && !assigned; ++v) {
                if (solution[v].empty()) { // Un véhicule à court terme peut prendre un seul client
                    solution[v].push_back(0); // Ajouter le dépôt
                    solution[v].push_back(client);
//                    solution[v].push_back(0); // Retourner au dépôt
                    assigned = true;
                }
            }
        }

        // Si le client n'est toujours pas assigné, il y a un problème de capacité
        if (!assigned) {
            throw std::runtime_error("Unable to assign all clients to vehicles.");
        }
    }

    // display the solution
    displaySolution(solution);

    return solution;
}

// Helper function pour vérifier si le véhicule peut prendre un autre client
bool TabouSearch::canAccommodate(const std::vector<int> &tour, int client, int vehicle) {
    float load = 0.0;
    // Calculer la charge actuelle du véhicule
    for (int i: tour) {
        load += config.Demand[i];
    }
    // Vérifier si le client peut être ajouté sans dépasser la capacité
/*
    cout << "load: " << load << " config.Demand[client]: " << config.Demand[client] << " config.Capacity[vehicle]: " << config.Capacity[vehicle] << endl;
*/
    return load + config.Demand[client] <= config.Capacity[vehicle];
}


float TabouSearch::run() {
    this->currentSolution = this->generateInitialSolution();
    this->bestSolution = this->currentSolution;
    this->bestCost = this->calculateCost(this->currentSolution);

    for (int iteration = 0; iteration < this->num_iterations; ++iteration) {
        auto candidateMoves = this->generateCandidateMoves();
        std::pair<int, int> bestMove;
        float bestMoveCost = std::numeric_limits<float>::max();

        for (const auto &move: candidateMoves) {
            if (!this->isTabou(move)) {
                // Appliquer temporairement le mouvement
                applyMove(move);
                float tempCost = this->calculateCost(this->currentSolution);

                // Annuler le mouvement
                applyMove(move);

                if (tempCost < bestMoveCost) {
                    bestMoveCost = tempCost;
                    bestMove = move;
                }
            }
        }

        // Si un mouvement améliorant a été trouvé, l'appliquer et mettre à jour la liste tabou
        if (bestMoveCost < this->bestCost) {
            applyMove(bestMove);
            this->bestCost = bestMoveCost;
            this->bestSolution = this->currentSolution;
            updateTabouList(bestMove);
        }

        // Gestion de la durée de vie de la liste tabou pourrait être ajoutée ici
    }

    return this->bestCost;
}

void TabouSearch::displaySolution(const std::vector<std::vector<int>> &solution) {
    std::cout << "Solution:\n";
    for (int i = 0; i < solution.size(); ++i) {
        if (solution[i].empty()) {
            continue;
        }
        std::cout << "Le véhicule " << i << " fait la tournée: ";
        for (int client: solution[i]) {
            std::cout << client << " ";
        }
        std::cout << "\n";
    }
}

float TabouSearch::calculateCost(const std::vector<std::vector<int>> &solution) {
    float cost = 0.0f;

    // ... Calculer le coût ici en fonction de la matrice de distance, des coûts fixes,
    // des pénalités de temps et de distance, et des demandes ...

    return cost;
}

// Génère une liste de mouvements candidats
std::vector<std::pair<int, int>> TabouSearch::generateCandidateMoves() {
    std::vector<std::pair<int, int>> moves;
    for (size_t i = 0; i < this->currentSolution.size() - 1; ++i) {
        for (size_t j = i + 1; j < currentSolution.size(); ++j) {
            moves.emplace_back(i, j);
        }
    }
    return moves;
}

// Applique un mouvement à la solution actuelle
void TabouSearch::applyMove(std::pair<int, int> move) {
    std::swap(currentSolution[move.first], currentSolution[move.second]);
}

bool TabouSearch::isTabou(const std::pair<int, int> &move) {
    for (auto &tabuMove: tabouList) {
        if (tabuMove == move) {
            return true;
        }
    }
    return false;
}

void TabouSearch::updateTabouList(const std::pair<int, int> &move) {
    // Ajouter un nouveau mouvement à la liste tabou et retirer les anciens si nécessaire
    tabouList.push_back(move);
    if (tabouList.size() > tabou_tenure) {
        tabouList.pop_front();
    }
}

void TabouSearch::decrementTabouTenure() {
    // Décrémenter la durée de vie des éléments de la liste tabou
}