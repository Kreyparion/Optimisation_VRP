#include "tabou_search.h"

#include <utility>

using namespace std;

// Implémentation des fonctions de la classe TabouSearch

// Initialisation des membres de la classe si nécessaire
TabouSearch::TabouSearch(Config config, int num_iterations, int tabou_tenure, bool verbose) :
        config(std::move(config)),
        num_iterations(num_iterations),
        tabou_tenure(tabou_tenure),
        verbose(verbose) {}

/*vector<vector<int>> TabouSearch::generateInitialSolution() {
    if (verbose) {
        cout << "Génération de la solution initiale..." << endl;
    }

    vector<vector<int>> solution(config.nbVehicle + config.nbShortTermVehicle);

    // Initialiser les tournées pour les véhicules
    for (int vehicule = 0; vehicule < config.nbVehicle + config.nbShortTermVehicle; ++vehicule) {
        solution[vehicule].push_back(0); // Ajouter le dépôt comme point de départ
    }

    // Assigner les clients aux véhicules à long terme en respectant la capacité
    for (int client = 1; client < config.nbVertex; ++client) {
        bool assigned = false;
        for (int vehicule = 0; vehicule < config.nbVehicle && !assigned; ++vehicule) {
            if (canAssignLongTerm(solution[vehicule], client, vehicule)) {
                if (verbose) {
                    cout << "Client: " << client << " assigné au véhicule long-term: " << vehicule << endl;
                }
                solution[vehicule].push_back(client);
                assigned = true;
            } else {
                if (verbose) {
                    cout << "Client: " << client << " non-assigné au vehicule long-term: " << vehicule << endl << endl;
                }
            }
        }

        // Si aucun véhicule à long terme ne peut prendre le client, essayer les véhicules à court terme
        if (!assigned) {
            for (int vehicule = config.nbVehicle; vehicule < solution.size() && !assigned; ++vehicule) {
                if (solution[vehicule].size() == 1 &&
                    canAssignShortTerm(client, vehicule)) { // Un véhicule à court terme peut prendre un seul client
                    if (verbose) {
                        cout << "Client: " << client << " assigné au véhicule short-term: " << vehicule
                             << endl;
                    }
                    solution[vehicule].push_back(client);
                    assigned = true;
                } else {
                    if (verbose) {
                        cout << "Client: " << client << " non-assigné au vehicule short-term: " << vehicule << endl
                             << endl;
                    }
                }
            }
        }

        // Si le client n'est toujours pas assigné, il y a un problème de capacité
        if (!assigned) {
            // affichage des infos pour debug
            if (verbose) {
                cout << "Client: " << client << " non-assigné à un véhicule." << endl;
            }
            throw runtime_error("Impossible d'assigner le client à un véhicule. Capacité insuffisante.");
        }
    }

    return solution;
}*/

vector<vector<int>> TabouSearch::generateInitialSolution() {
    if (verbose) {
        cout << "Génération de la solution initiale avec l'heuristique du plus proche voisin..." << endl;
    }

    vector<vector<int>> solution(config.nbVehicle + config.nbShortTermVehicle,
                                 vector<int>(1, 0)); // Initialisation des tournées avec le dépôt

    // Initialiser un vecteur pour suivre quels clients ont été assignés
    vector<bool> assignedClients(config.nbVertex, false); // false par défaut, true si assigné

    // Pour chaque véhicule
    for (int vehicule = 0; vehicule < config.nbVehicle + config.nbShortTermVehicle; ++vehicule) {
        bool canAssignMore = true; // Contrôle si le véhicule peut prendre plus de clients

        while (canAssignMore) {
            int lastClient = solution[vehicule].back();
            int nearestClient = -1;
            double nearestDistance = numeric_limits<double>::max();

            // Trouver le client non assigné le plus proche
            for (int client = 1; client < config.nbVertex; ++client) {
                if (!assignedClients[client]) {
                    double distance = config.dist[lastClient * config.nbVertex +
                                                  client]; // À implémenter selon votre structure de données
                    if (distance < nearestDistance) {
                        nearestClient = client;
                        nearestDistance = distance;
                    }
                }
            }

            // Essayer d'assigner le client le plus proche au véhicule courant
            if (nearestClient != -1 &&
                canAssignClient(solution[vehicule], nearestClient, vehicule)) { // `canAssignClient` à adapter
                solution[vehicule].push_back(nearestClient);
                assignedClients[nearestClient] = true;
            } else {
                canAssignMore = false; // Aucun client proche ou capacité atteinte
            }
        }
    }

    // Vérifier si tous les clients ont été assignés
    if (find(assignedClients.begin() + 1, assignedClients.end(), false) != assignedClients.end()) {
        displaySolution(solution);
        throw runtime_error(
                "Impossible d'assigner tous les clients. Capacité insuffisante ou contraintes trop strictes.");
    }

    return solution;
}

bool TabouSearch::canAssignClient(vector<int> &vector, int client, int vehicule) {
    return vehicule < config.nbVehicle ? canAssignLongTerm(vector, client, vehicule) :
    canAssignShortTerm(vector, client,vehicule);
}

// Helper function pour vérifier si le véhicule peut prendre un autre client
bool TabouSearch::canAssignLongTerm(const vector<int> &tour, int client, int vehicle) {
    if (verbose) {
        cout << "Essai du client: " << client << " avec le véhicule long-term: " << vehicle << endl << endl;
    }

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

    if (verbose) {
        cout << "Charge avant le client: " << load << " Distance totale avant le client: " << totalDistance
             << endl;
        cout << "Charge apres: " << load + config.Demand[client] << " Distance totale apres: " << totalDistance
             << " Temps total: " << totalTime << endl;
        cout << "Capacité: " << config.Capacity[vehicle] << " Limite de temps: " << config.HardTimeLimit[vehicle]
             << endl << endl;
    }

    // Vérifier la capacité et la limite de temps
    return load + config.Demand[client] <= config.Capacity[vehicle] && totalTime <= config.HardTimeLimit[vehicle];
}


bool TabouSearch::canAssignShortTerm(const vector<int> &tour, int client, int vehicle) {
    if (verbose) {
        cout << "Essai du client: " << client << " avec le véhicule short-term: " << vehicle << endl;
    }

    if (tour.size() == 2) {
        // Si le véhicule a un client, il est complet
        if (verbose) {
            cout << "Le véhicule short-term est plein, donc le client ne peut être assigné." << endl;
        }
        return false;
    }

    float limiteDistance = config.HardDistanceLimitShortTermVehicle[vehicle];

    // Si le véhicule a une limite de distance, vérifier cette limite
    if (limiteDistance > 0.0) {
        float totalDistance = config.dist[0 * config.nbVertex + client];
        return totalDistance <= limiteDistance;
    }

    // Si le véhicule à court terme n'a de limite de distance hard, retourner vrai
    return true;
}

float TabouSearch::run() {
    cout << "Début du programme" << endl << endl;
/*    vector<vector<int>> solution_opti = {
            {0, 10, 8, 6, 9, 7, 5},
            {0, 4,  2, 3},
            {0},
            {0, 1},
            {0}
    };*/

/*    cout << "Solution optimale: " << endl << endl;
    displaySolution(solution_opti);
    cout << endl;
    float cost = calculateCost(solution_opti);
    cout << "Prix optimal : " << cost << endl << endl;*/

    this->currentSolution = this->generateInitialSolution();
    cout << "Solution initiale: " << endl << endl;
    displaySolution(this->currentSolution);
    this->bestSolution = this->currentSolution;
    this->bestCost = this->calculateCost(this->currentSolution);
    cout << "Prix initial: " << this->bestCost << endl << endl;

    /*for (int iteration = 0; iteration < this->num_iterations; ++iteration) {
        auto candidateMoves = this->generateCandidateMoves();
        cout << "Itération " << iteration << " - Nombre de mouvements candidats: " << candidateMoves.size() << endl;
        pair<int, int> bestMove;
        float bestMoveCost = numeric_limits<float>::max();

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
    }*/

    return this->bestCost;
}


void TabouSearch::displaySolution(const vector<vector<int>> &solution) const {
    if (verbose) {
        cout << "Affichage de la solution..." << endl;
    }

    if (solution.empty()) {
        cout << "La solution est vide." << endl;
        return;
    }

    for (int i = 0; i < solution.size(); ++i) {
        if (solution[i].size() == 1) {
            cout << "Le véhicule " << i << " ne fait pas de tournée." << endl;
            continue;
        }

        cout << "Le véhicule " << i << " fait la tournée: ";

        for (int j = 0; j < solution[i].size(); ++j) {
            cout << solution[i][j];
            if (j < solution[i].size() - 1) {
                cout << "->";
            }
        }
        // Vérifiez si le véhicule est long-term ou short-term
        if (i < config.nbVehicle) {
            // Pour les véhicules long-term, ajoutez "->0" pour montrer le retour au dépôt
            cout << "->0";
        } else {
            // Pour les véhicules short-term, vérifiez s'ils ont plus d'un client
            if (solution[i].size() > 2) {
                cout << " (Attention: les véhicules short-term devraient avoir un seul client)";
            }
        }
        cout << "\n";
    }
}

float TabouSearch::calculateCost(const vector<vector<int>> &solution) {
    if (verbose) {
        cout << "Calcul du coût de la solution..." << endl;
    }
    float totalCost = 0.0f;
    for (const auto &vehicleTour: solution) {
        if (vehicleTour.size() == 1) continue; // Skip empty tours

        float tourCost = 0.0f;
        int previousLocation = vehicleTour.front(); // Starting at the depot, assumed to be 0 for simplicity

        // Determine vehicle index and type (long-term or short-term)
        int vehicleIndex = &vehicleTour - &solution[0]; // Get the vehicle index based on the address difference
        bool isLongTerm = vehicleIndex < config.nbVehicle;

        // Add fixed cost for using the vehicle
        if (isLongTerm) {
            // Long-term vehicle
            tourCost += config.fixedCostVehicle[vehicleIndex];
        } else {
            // Short-term vehicle, adjust index for short-term list
            int shortTermIndex = vehicleIndex - config.nbVehicle;
            tourCost += config.fixedCostShortTermVehicle[shortTermIndex];
            if (verbose) {
                cout << "Coût de la tournée du véhicule short-term " << vehicleIndex << ": " << tourCost << endl;
                cout << "Coût fixe : " << config.fixedCostShortTermVehicle[shortTermIndex] << endl;
                cout << "Coût total: " << totalCost << endl;
            }
            totalCost += tourCost;
            continue; // Skip the rest of the cost calculation for short-term vehicles
        }

        float totalDistance = 0.0f;
        float totalTime = 0.0f;

        for (size_t i = 1; i < vehicleTour.size(); ++i) {
            int currentLocation = vehicleTour[i];
            // Calculate distance
            float distance = config.dist[previousLocation * config.nbVertex + currentLocation];
            totalDistance += distance;

            // Calculate time based on speed and distance (assuming time = distance / speed)
            float time = distance / config.speed[vehicleIndex]; // Ensure config.speed is defined
            totalTime += time;

            previousLocation = currentLocation;
        }

        float distanceBack = config.dist[previousLocation * config.nbVertex + 0];
        totalDistance += distanceBack; // Add the cost of returning to the depot

        float time = distanceBack / config.speed[vehicleIndex]; // Ensure config.speed is defined
        totalTime += time;

        // Add penalties if soft limits are exceeded
        if (totalDistance > config.SoftDistanceLimit[vehicleIndex]) {
            float extraDistance = totalDistance - config.SoftDistanceLimit[vehicleIndex];
            tourCost +=
                    extraDistance * config.distancePenalty[vehicleIndex]; // Ensure config.distancePenalty is defined
        }
        if (totalTime > config.SoftTimeLimit[vehicleIndex]) {
            float extraTime = totalTime - config.SoftTimeLimit[vehicleIndex];
            tourCost += extraTime * config.timePenalty[vehicleIndex]; // Ensure config.timePenalty is defined
        }

        totalCost += tourCost;
        if (verbose) {
            cout << "Coût de la tournée du véhicule long-term " << vehicleIndex << ": " << tourCost << endl;
            cout << "Coût total: " << totalCost << endl;
            cout << "Coût fixe : " << config.fixedCostVehicle[vehicleIndex] << endl;
            cout << "Temps total de la tournée: " << totalTime << endl;
            cout << "Distance totale de la tournée: " << totalDistance << endl;
            cout << "Temps extra: " << totalTime - config.SoftTimeLimit[vehicleIndex] << endl;
            cout << "Distance extra: " << totalDistance - config.SoftDistanceLimit[vehicleIndex] << endl;
            cout << "Prix temps extra: "
                 << max((totalTime - config.SoftTimeLimit[vehicleIndex]) * config.timePenalty[vehicleIndex], 0.0f)
                 << endl;
            cout << "Prix distance extra: " << max((totalDistance - config.SoftDistanceLimit[vehicleIndex]) *
                                                   config.distancePenalty[vehicleIndex], .0f) << endl << endl;
        }
    }
    return totalCost;
}

/*void TabouSearch::generateCandidateMoves() {
    vector<Move> candidateMoves;

    // Générer des mouvements pour les véhicules à long terme
    for (int vehicle = 0; vehicle < config.nbVehicle; ++vehicle) {
        // Pour chaque client non encore assigné ou pour des échanges possibles
        for (int client = 1; client < config.nbVertex; ++client) {
            if (canAssignLongTerm(currentSolution[vehicle], client, vehicle)) {
                // Ajouter le client, remplacer un client existant, ou échanger entre tournées
                Move move = ...; // définir le mouvement
                candidateMoves.push_back(move);
            }
        }
    }

    // Générer des mouvements pour les véhicules à court terme
    for (int vehicle = config.nbVehicle; vehicle < config.nbVehicle + config.nbShortTermVehicle; ++vehicle) {
        // Même logique mais adaptée aux contraintes des véhicules à court terme
        for (int client = 1; client < config.nbVertex; ++client) {
            if (canAssignShortTerm(client, vehicle)) {
                Move move = ...; // définir le mouvement
                candidateMoves.push_back(move);
            }
        }
    }

    // Évaluer et sélectionner les meilleurs mouvements
    evaluateAndSelectMoves(candidateMoves);
}*/

// Génère une liste de mouvements candidats
vector<pair<pair<int, int>, pair<int, int>>> TabouSearch::generateCandidateMoves() {
    vector<pair<pair<int, int>, pair<int, int>>> moves;

    // Parcourir chaque tournée pour identifier les mouvements potentiels
    for (size_t i = 0; i < this->currentSolution.size(); ++i) {
        for (size_t j = 0; j < this->currentSolution[i].size(); ++j) {
            // Déplacement d'un client vers une autre tournée
            for (size_t k = 0; k < this->currentSolution.size(); ++k) {
                if (k != i) { // Ne pas déplacer dans la même tournée
                    if (isValidMove(i, j, k)) { // Vérifier si le mouvement est valide
                        moves.push_back({{i, j},
                                         {k, -1}}); // -1 indique un ajout sans position spécifique dans la tournée cible
                    }
                }
            }

            // Échange de clients entre tournées différentes ou au sein de la même tournée
            /*for (size_t k = 0; k < this->currentSolution.size(); ++k) {
                for (size_t l = (k == i ? j+1 : 0); l < this->currentSolution[k].size(); ++l) {
                    if (isValidMove(i, j, k, l)) { // Vérifier si l'échange est valide
                        moves.push_back({{i, j}, {k, l}});
                    }
                }
            }*/
        }
    }

    return moves;
}

bool TabouSearch::isValidMove(int client, int fromVehicle, int toVehicle) {
    // Identifier si les véhicules sont à long ou court terme
    bool isFromVehicleLongTerm = fromVehicle < config.nbVehicle;
    bool isToVehicleLongTerm = toVehicle < config.nbVehicle;

    // Calculer la demande totale et le temps pour le véhicule de destination
    float load = 0.0, totalDistance = 0.0, totalTime;
    vector<int> tour; // Ici, vous devez récupérer la tournée actuelle du véhicule de destination

    // Pour simplifier, supposons tour contient les clients actuels du véhicule de destination
    // tour = solution[toVehicle]; // Supposons une structure où solution est un vecteur de vecteurs d'entiers

    int prevLocation = 0; // Le dépôt est le point de départ

    // Calculer la charge, la distance totale et le temps pour la tournée actuelle
    for (int location: tour) {
        load += config.Demand[location];
        totalDistance += config.dist[prevLocation * config.nbVertex + location];
        prevLocation = location;
    }

    // Ajouter les calculs pour le nouveau client
    load += config.Demand[client];
    totalDistance += config.dist[prevLocation * config.nbVertex + client] +
                     config.dist[client * config.nbVertex + 0]; // Comprend le retour au dépôt
    totalTime = totalDistance / config.speed[toVehicle];

    // Vérification pour un véhicule long terme
    if (isToVehicleLongTerm) {
        if (load > config.Capacity[toVehicle] || totalTime > config.HardTimeLimit[toVehicle]) {
            return false; // Ne respecte pas la capacité ou la limite de temps
        }
    } else { // Vérification pour un véhicule court terme
        // Vérifiez simplement la distance jusqu'au client puisque canAssignShortTerm ne prend en compte que cela
        float distanceToClient = config.dist[0 * config.nbVertex + client];
        if (config.HardDistanceLimitShortTermVehicle[toVehicle] > 0 &&
            distanceToClient > config.HardDistanceLimitShortTermVehicle[toVehicle]) {
            return false; // Ne respecte pas la limite de distance hard
        }
    }

    // Si toutes les vérifications sont passées, le mouvement est valide
    return true;
}

// Applique un mouvement à la solution actuelle
void TabouSearch::applyMove(pair<int, int> move) {
    swap(currentSolution[move.first], currentSolution[move.second]);
}

bool TabouSearch::isTabou(const pair<int, int> &move) {
    for (auto &tabuMove: tabouList) {
        if (tabuMove == move) {
            return true;
        }
    }
    return false;
}

void TabouSearch::updateTabouList(const pair<int, int> &move) {
    // Ajouter un nouveau mouvement à la liste tabou et retirer les anciens si nécessaire
    tabouList.push_back(move);
    if (tabouList.size() > tabou_tenure) {
        tabouList.pop_front();
    }
}

void TabouSearch::decrementTabouTenure() {
    // Décrémenter la durée de vie des éléments de la liste tabou
}
