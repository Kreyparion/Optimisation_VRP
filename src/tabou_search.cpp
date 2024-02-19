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
    for (int vehicule = 0; vehicule < config.nbVehicle; ++vehicule) {
        solution[vehicule].push_back(0); // Ajouter le dépôt comme point de départ
    }

    // Assigner les clients aux véhicules à long terme en respectant la capacité
    for (int client = 1; client < config.nbVertex; ++client) {
        bool assigned = false;
        for (int vehicule = 0; vehicule < config.nbVehicle && !assigned; ++vehicule) {
            if (canAccommodateLongTerm(solution[vehicule], client, vehicule)) {
                cout << "Client: " << client << " assigné au vehicule long-term: " << vehicule << endl << endl;
                solution[vehicule].push_back(client);
                assigned = true;
            } else {
                cout << "Client: " << client << " non-assigné au vehicule long-term: " << vehicule << endl << endl;
            }
        }

        // Si aucun véhicule à long terme ne peut prendre le client, essayer les véhicules à court terme
        if (!assigned) {
            for (int vehicule = config.nbVehicle; vehicule < solution.size() && !assigned; ++vehicule) {
                if (solution[vehicule].empty() &&
                    canAccommodateShortTerm(client, vehicule)) { // Un véhicule à court terme peut prendre un seul client
                    cout << "Client: " << client << " assigné au vehicule short-term: " << vehicule << endl << endl;
                    solution[vehicule].push_back(0); // Ajouter le dépôt
                    solution[vehicule].push_back(client);
//                    solution[vehicule].push_back(0); // Retourner au dépôt
                    assigned = true;
                } else {
                    cout << "Client: " << client << " non-assigné au vehicule short-term: " << vehicule << endl << endl;
                }
            }
        }

        // Si le client n'est toujours pas assigné, il y a un problème de capacité
        if (!assigned) {
            // affichage des infos pour debug
            cout << "Client: " << client << " non-assigné à un véhicule." << endl;
            cout << "Solution actuelle: " << endl;
            displaySolution(solution);
            throw std::runtime_error("Unable to assign all clients to vehicles.");
        }
    }

    // display the solution
    displaySolution(solution);

    return solution;
}

// Helper function pour vérifier si le véhicule peut prendre un autre client
bool TabouSearch::canAccommodateLongTerm(const std::vector<int> &tour, int client, int vehicle) {

    float load = 0.0, totalDistance = 0.0, totalTime;
    int prevLocation = 0; // Supposons que le dépôt est le point de départ

    // Calculer la charge, la distance totale et le temps pour la tournée actuelle
    for (int location: tour) {
        load += config.Demand[location];
        totalDistance += config.dist[prevLocation * config.nbVertex + location];
        prevLocation = location;
    }

    totalDistance += config.dist[prevLocation * config.nbVertex + client] +
                     config.dist[client * config.nbVertex + 0]; // Ajout du retour au dépôt
    totalTime = totalDistance / config.speed[vehicle];

    cout << "Essai du client: " << client << " avec le véhicule long-term: " << vehicle << endl;
    cout << "Charge avant le client: " << load << " Distance totale avant le client: " << totalDistance << endl;
    cout << "Charge apres: " << load + config.Demand[client] << " Distance totale apres: " << totalDistance << " Temps total: " << totalTime << endl;
    cout << "Capacité: " << config.Capacity[vehicle] << " Limite de temps: " << config.HardTimeLimit[vehicle] << endl;
    // Vérifier la capacité et la limite de temps
    return load + config.Demand[client] <= config.Capacity[vehicle] && totalTime <= config.HardTimeLimit[vehicle];
}

bool TabouSearch::canAccommodateShortTerm(int client, int vehicle) {
//    cout << "Essai du client: " << client << " avec le véhicule short-term: " << vehicle << endl;
    // Si le véhicule a une limite de distance, vérifier cette limite
    if (config.HardDistanceLimitShortTermVehicle[vehicle] > 0.0) {
        float totalDistance = config.dist[0 * config.nbVertex + client];
        return totalDistance <= config.HardDistanceLimitShortTermVehicle[vehicle];
    }

    // Si le véhicule à court terme n'a de limite de distance hard, retourner vrai
    return true;
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