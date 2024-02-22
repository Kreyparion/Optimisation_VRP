#pragma once
#include "tabou_search.h"
#include <utility>

using namespace std;

// Implémentation des fonctions de la classe TabouSearch

// Initialisation des membres de la classe si nécessaire
TabouSearch::TabouSearch(Config config, int num_iterations, int tabouDuration, int verbose):
        config(std::move(config)),
        num_iterations(num_iterations),
        tabouDuration(tabouDuration),
        verbose(verbose) {}

vector<vector<int>> TabouSearch::generateInitialSolution() {
    if (verbose>1) {
        cout << "Génération de la solution initiale..." << endl;
    }

    vector<vector<int>> solution(config.nbVehicle + config.nbShortTermVehicle); // Initialiser la solution avec le nombre de véhicules

    // Initialiser les tournées pour les véhicules à long terme
    for (int vehicule = 0; vehicule < config.nbVehicle + config.nbShortTermVehicle; ++vehicule) {
        solution[vehicule].push_back(0); // Ajouter le dépôt comme point de départ
    }

    // Assigner les clients aux véhicules à long terme en respectant la capacité
    std::vector<int> clients(config.nbVertex-1);
    for(int i=0; i<config.nbVertex-1; i++){
        clients[i] = i+1;
    }
    std::random_shuffle(clients.begin(), clients.end());
    for (int client: clients) {
        bool assigned = false;
        for (int vehicule = 0; vehicule < config.nbVehicle && !assigned; ++vehicule) {
            if (canAssignLongTerm(solution[vehicule], client, vehicule)) {
                if (verbose>1) {
                    cout << "Client: " << client << " assigné au véhicule long-term: " << vehicule << endl;
                }
                solution[vehicule].push_back(client);
                assigned = true;
            }
        }

        // Si aucun véhicule à long terme ne peut prendre le client, essayer les véhicules à court terme
        if (!assigned) {
            int n = solution.size();
            for (int vehicule = config.nbVehicle; vehicule < n && !assigned; ++vehicule) {
                if (solution[vehicule].size() == 1 &&
                    canAssignShortTerm(solution[vehicule], client,
                                       vehicule)) { // Un véhicule à court terme peut prendre un seul client
                    if (verbose>1) {
                        cout << "Client: " << client << " assigné au véhicule short-term: " << vehicule
                             << endl;
                    }
                    solution[vehicule].push_back(client);
                    assigned = true;
                } else {
                    if (verbose>1) {
                        cout << "Client: " << client << " non-assigné au vehicule short-term: " << vehicule << endl
                             << endl;
                    }
                }
            }
        }

        // Si le client n'est pas assigné à un véhicule à long terme, lui attribuer un nouveau véhicule à court terme
        if (!assigned) {
            vector<int> shortTermTour = {0, client}; // Créer une nouvelle tournée pour un véhicule à court terme
            solution.push_back(shortTermTour); // Ajouter cette nouvelle tournée à la solution
            if (verbose>1) {
                cout << "Client: " << client << " assigné à un nouveau véhicule short-term." << endl;
            }
        }
    }
    solution.push_back({0});
    solution.push_back({0});
    solution.push_back({0});
    solution.push_back({0});

    return solution;
}

// Helper function pour vérifier si le véhicule peut prendre un autre client
bool TabouSearch::canAssignLongTerm(const vector<int> &tour, int client, int vehicle) {
    if (verbose>1) {
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

    if (verbose>1) {
        cout << "CLient: " << client << " Véhicule: " << vehicle << endl;
        cout << "Charge avant le client: " << load << " Distance totale avant le client: " << totalDistance
             << endl;
        cout << "Charge apres: " << load + config.Demand[client] << " Distance totale apres: " << totalDistance
             << " Temps total: " << totalTime << endl;
        cout << "Capacité: " << config.Capacity[vehicle] << " Limite de temps: " << config.HardTimeLimit[vehicle]
             << endl << endl;
    }

    // Vérifier la capacité et la limite de temps
    return (load + config.Demand[client]) <= config.Capacity[vehicle] && totalTime <= config.HardTimeLimit[vehicle];
}


bool TabouSearch::canAssignShortTerm(const vector<int> &tour, int client, int vehicle) {
    if (verbose>1) {
        cout << "Essai du client: " << client << " avec le véhicule short-term: " << vehicle << endl;
    }

    if (tour.size() == 2) {
        // Si le véhicule a un client, il est complet
        if (verbose>1) {
            cout << "Le véhicule short-term est plein, donc le client ne peut être assigné." << endl;
        }
        return false;
    }

    float limiteDistance = config.HardDistanceLimitShortTermVehicle[vehicle-config.nbVehicle];

    // Si le véhicule a une limite de distance, vérifier cette limite
    if (limiteDistance > 0.0) {
        float totalDistance = config.dist[0 * config.nbVertex + client];
        return totalDistance <= limiteDistance;
    }

    // Si le véhicule à court terme n'a de limite de distance hard, retourner vrai
    return true;
}

float TabouSearch::run() {
    if(verbose>0){
        cout << "Début du programme" << endl << endl;
    }

    // Génération de la solution initiale
    this->currentSolution = this->generateInitialSolution();
    if(verbose>0){
        cout << "Solution initiale générée." << endl << endl;
        displaySolution(this->currentSolution);
    }

    // Initialisation du meilleur coût et de la meilleure solution
    this->bestSolution = this->currentSolution;
    this->bestCost = this->calculateCost(this->currentSolution);
    if(verbose>0){
        cout << "Coût initial: " << this->bestCost << endl << endl;
    }
    // Initialisation de la liste tabou
    this->tabouList.clear();

    int iterationsWithoutImprovement = 0; // Ajout d'un compteur pour les itérations sans amélioration

    for (int iteration = 0; iteration < this->num_iterations; ++iteration) {
        if(verbose>0){
            cout << "Début de l'itération: " << iteration << endl;
        }

        auto candidateMoves = this->generateCandidateMoves(currentSolution);
        if(verbose>0){
            cout << "Solution à l'itération : " << iteration << endl;
            displaySolution(this->currentSolution);
        
            cout << "Nombre de mouvements candidats générés: " << candidateMoves.size() << endl;
        }

        pair<pair<int, int>, pair<int, int>> bestMove({{-1, -1},
                                                       {-1, -1}});

        float bestMoveCost = numeric_limits<float>::max();

        // Ajout pour le débogage :
        if(verbose>1){
            for (const auto &move: candidateMoves) {
                cout << "Mouvement candidat: (" << move.first.first << "," << move.first.second << ") -> ("
                    << move.second.first << "," << move.second.second << ")" << endl;
            }
        }
        for (const auto &move: candidateMoves) {
            if (!this->isTabou(move)) {
                // Appliquer temporairement le mouvement pour évaluer son coût
                std::vector<std::vector<int>> tempSolution = applyMove(move);
                float tempCost = this->calculateCost(tempSolution);
                if(verbose>1){
                    cout << "Solution temporaire après application du mouvement: ";
                    cout << "(" << move.first.first << "," << move.first.second << ") -> ";
                    cout << "(" << move.second.first << "," << move.second.second << ")" << endl;
                    displaySolution(tempSolution);
                    cout << "Coût temporaire après application du mouvement: " << tempCost << endl;
                }

                if (tempCost < bestMoveCost) {
                    bestMoveCost = tempCost;
                    bestMove = move;
                    if(verbose>0){
                        cout << "Nouveau meilleur mouvement trouvé avec un coût de: " << bestMoveCost << endl;
                    }
                }
            }
        }

        // Si un meilleur mouvement a été trouvé, l'appliquer et mettre à jour la solution et la liste tabou
        if (bestMove.first.first != -1) {
            if(verbose>0){
                cout << "Meilleur mouvement sélectionné pour application: ";
                cout << "(" << bestMove.first.first << "," << bestMove.first.second << ") -> ";
                cout << "(" << bestMove.second.first << "," << bestMove.second.second << ")" << " avec un coût de: "
                    << bestMoveCost << endl;
            }
            this->currentSolution = applyMove(bestMove);
            updateTabouList(bestMove);

            // Ajout pour le débogage :
            if(verbose>0){
                cout << "Mouvement appliqué et ajouté à la liste tabou." << endl;
            }

            if (bestMoveCost < this->bestCost) {
                this->bestCost = bestMoveCost;
                this->bestSolution = this->currentSolution;
                if (verbose>0) {
                    cout << "Nouveau meilleur coût trouvé: " << this->bestCost << " à l'itération " << iteration << endl;
                }
                iterationsWithoutImprovement = 0; // Réinitialisation du compteur
            } else {
                iterationsWithoutImprovement++;
                if (verbose>1) {
                    cout << "Itération " << iteration << " sans amélioration. Total sans amélioration: "
                         << iterationsWithoutImprovement << endl;
                }
            }
        } else {
            iterationsWithoutImprovement++;
            if (verbose>1) {
                cout << "Aucun mouvement améliorant trouvé." << endl;
            }
        }

        // Arrêter la recherche si aucune amélioration n'a été trouvée après 20 itérations
        if (iterationsWithoutImprovement >= 20) {
            if (verbose>0) {
                cout << "Aucune amélioration après 20 itérations. Arrêt de la recherche." << endl;
                displaySolution(this->currentSolution);
                this->bestCost = this->calculateCost(this->currentSolution);
                cout << "Coût final: " << this->bestCost << endl << endl;
            }
            break;
        }

        // Gérer la taille de la liste tabou pour éviter qu'elle ne devienne trop grande
        int tabouListSize = this->tabouList.size();
        if (tabouListSize > this->tabouListMaxSize) {
            this->tabouList.erase(this->tabouList.begin());
            // Ajout pour le débogage :
            if (verbose>1) {
                cout << "Mouvement retiré de la liste tabou pour gérer la taille." << endl;
            }
        }

    }
    if(verbose>0){
        cout << "Recherche tabou terminée." << endl;
    }
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
    int n = solution.size();
    for (int i = 0; i < n; ++i) {
        int ni = solution[i].size();
        if (ni == 1) {
            cout << "Le véhicule " << i << " ne fait pas de tournée." << endl;
            continue;
        }

        cout << "Le véhicule " << i << " fait la tournée: ";

        for (int j = 0; j < ni; ++j) {
            cout << solution[i][j];
            if (j < ni - 1) {
                cout << "->";
            }
        }
        // Vérifiez si le véhicule est long-term ou short-term
        if (i < config.nbVehicle) {
            // Pour les véhicules long-term, ajoutez "->0" pour montrer le retour au dépôt
            cout << "->0";
        } else {
            // Pour les véhicules short-term, vérifiez s'ils ont plus d'un client
            if (ni > 2) {
                cout << " (Attention: les véhicules short-term devraient avoir un seul client)";
            }
        }
        cout << "\n";
    }
}

float TabouSearch::calculateCost(const vector<vector<int>> &sol) {
    float score = 0.0;
    std::vector<bool> visited(config.nbVertex-1, false);
    int n = sol.size();
    if(n < config.nbVehicle + config.nbShortTermVehicle){
        return 1000000.0;
    }
    for(int i=0; i<config.nbVehicle; i++){
        int ni = sol[i].size();
        if(ni > 1){
            int capacity = 0;
            float distance = 0.0;
            int last = 0;
            for(int j=1; j<ni; j++){
                int vertex = sol[i][j];
                if(vertex <= 0 || vertex >= config.nbVertex){
                    return 1000000.0;
                }
                if(visited[vertex-1]){
                    return 1000000.0;
                }
                visited[vertex-1] = true;
                capacity += config.Demand[vertex];
                distance += config.dist[last*config.nbVertex+vertex];
                last = vertex;
            }
            distance += config.dist[last*config.nbVertex];
            if(capacity > config.Capacity[i]){
                return 1000000.0;
            }
            if(distance > config.SoftDistanceLimit[i]){
                score += config.distancePenalty[i] * (distance - config.SoftDistanceLimit[i]);
            }
            float time = distance / config.speed[i];
            if(time > config.HardTimeLimit[i]){
                return 1000000.0;
            }
            if(time > config.SoftTimeLimit[i]){
                score += config.timePenalty[i] * (time - config.SoftTimeLimit[i]);
            }
            score += config.fixedCostVehicle[i];
        }
    }
    for(int i=config.nbVehicle; i<config.nbVehicle+config.nbShortTermVehicle; i++){
        int ni = sol[i].size();
        if(ni > 1){
            if (ni > 2){
                return 1000000.0;
            }
            int vertex = sol[i][1];
            if(vertex <= 0 || vertex >= config.nbVertex){
                return 1000000.0;
            }
            float distance = config.dist[0*config.nbVertex+vertex];
            if(distance > config.HardDistanceLimitShortTermVehicle[i-config.nbVehicle]){
                return 1000000.0;
            }
            visited[vertex-1] = true;
            score += config.fixedCostShortTermVehicle[i-config.nbVehicle];
        }
    }
    if (n > config.nbVehicle + config.nbShortTermVehicle){
        for(int i=config.nbVehicle+config.nbShortTermVehicle; i<n; i++){
            int ni = sol[i].size();
            if(ni > 1){
                if(ni > 2){
                    return 1000000.0;
                }
                int vertex = sol[i][1];
                if(vertex <= 0 || vertex >= config.nbVertex){
                    return vertex;
                }
                visited[vertex-1] = true;
                score += 350.0;
            }
        }
    }
    for(int i=0; i<config.nbVertex-1; i++){
        if(!visited[i]){
            return 1000000.0;
        }
    }
    return score;
}

vector<pair<pair<int, int>, pair<int, int>>> TabouSearch::generateCandidateMoves(const vector<vector<int>> &solution) {
    vector<pair<pair<int, int>, pair<int, int>>> moves;
    int n = solution.size();
    for (int i = 0; i < n; ++i) {
        int ni = solution[i].size();
        for (int j = 1; j < ni; ++j) { // Commence à 1 pour ignorer le dépôt
            int clientA = solution[i][j];

            // Générer des mouvements de transfert (ajouter à la fin d'une autre tournée)
            for (int k = 0; k < n; ++k) {
                int nk = solution[k].size();
                if (k < config.nbVehicle || nk == 1) { // Ne pas ajouter à un véhicule à court terme
                    if (k != i) { // Assurez-vous de ne pas choisir la même tournée
                        moves.push_back({{i, j},
                                        {k, -1}}); // -1 indique l'ajout à la fin de la tournée k
                    }
                }
            }
            
            // Générer des mouvements d'échange comme précédemment
            for (int k = i; k < n; ++k) {
                int nk = solution[k].size();
                for (int l = (i == k ? j + 1 : 1); l < nk; ++l) {
                    int clientB = solution[k][l];
                    if (clientA != 0 && clientB != 0) {
                        moves.push_back({{i, j},
                                         {k, l}});
                    }
                }
            }
            /*
            for(int k=0; k<n; k++){
                int nk = solution[k].size();
                for(int l=1; l<nk; l++){

                    moves.push_back({{i, j}, {k, l}});
                    
                }
                
            }*/
        }
    }
    return moves;
}




// Applique un mouvement à la solution actuelle
std::vector<std::vector<int>> TabouSearch::applyMove(pair<pair<int, int>, pair<int, int>> move) {
    // Extraire les indices du premier et du second client à échanger
    std::vector<std::vector<int>> newSolution = currentSolution;

    int vehicle1 = move.first.first;
    int position1 = move.first.second;
    int vehicle2 = move.second.first;
    int position2 = move.second.second;
    int currentSolutionSize = currentSolution.size();
    int vehicle1Size = currentSolution[vehicle1].size();
    int vehicle2Size = currentSolution[vehicle2].size();
    

    // Vérifier si les positions sont valides avant de tenter le swap
    
    if (vehicle1 < currentSolutionSize && position1 < vehicle1Size &&
        vehicle2 < currentSolutionSize && position2 < vehicle2Size) {
        int client = currentSolution[vehicle1][position1];
        if (position2 < 0) {
            
            // Supprimer le client de la tournée du véhicule 1
            newSolution[vehicle1].erase(newSolution[vehicle1].begin() + position1);
            // Ajouter le client à la fin de la tournée du véhicule 2
            newSolution[vehicle2].push_back(client);
        }

        // Appliquer le swap directement sur les éléments de currentSolution

        else{
            std::swap(newSolution[vehicle1][position1], newSolution[vehicle2][position2]);
        }
    } else {
        // Gérer l'erreur ou le cas invalide ici
        if (verbose>1){
            std::cout << "Mouvement invalide : indices hors limites." << std::endl;
        }
    }
    return newSolution;
}

// Vérifie si un mouvement est dans la liste tabou
bool TabouSearch::isTabou(pair<pair<int, int>, pair<int, int>> move) {
    for (const auto &tabouMove: tabouList) {
        if (tabouMove.first == move) {
            return true; // Le mouvement est trouvé dans la liste tabou
        }
    }
    return false; // Le mouvement n'est pas tabou
}

// Ajoute un mouvement à la liste tabou avec une durée spécifiée
void TabouSearch::addToTabouList(pair<pair<int, int>, pair<int, int>> move) {
    int tabouListSize = tabouList.size();
    if (tabouListSize >= tabouListMaxSize) {
        tabouList.pop_front(); // Supprime le plus ancien si la liste est pleine
    }
    tabouList.push_back(make_pair(move, tabouDuration)); // Ajoute le nouveau mouvement
}

// Réduit la durée de vie des mouvements tabous et supprime ceux expirés
void TabouSearch::decrementTabouList() {
    auto it = tabouList.begin();
    while (it != tabouList.end()) {
        --(it->second); // Décrémente la durée de vie
        if (it->second <= 0) {
            it = tabouList.erase(it); // Supprime si expiré
        } else {
            ++it;
        }
    }
}

std::vector<std::vector<int>> TabouSearch::getBestSolution() const {
    std::vector<std::vector<int>> solution;
    int n = bestSolution.size();
    for (int i = 0; i < n; ++i) {
        solution.push_back({});
        int ni = bestSolution[i].size();
        for (int j = 1; j < ni; ++j) {
            solution[i].push_back(bestSolution[i][j]);
        }
    }
    return solution;
}

// Mettre à jour la liste tabou après chaque itération
void TabouSearch::updateTabouList(pair<pair<int, int>, pair<int, int>> move) {
    addToTabouList(move);
    decrementTabouList();
}