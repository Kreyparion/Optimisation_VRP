#include "tabou_search.h"

// Structure pour une solution
struct TabouSolution {
    std::vector<int> tour; // Représentation d'une solution (e.g., ordre de visite des clients)
    float cost{}; // Coût de la solution
};

float evaluate_permutation_tabou(Config& config, Permutation permutation){
    float cost = 0.0;
    if (permutation.empty()){
        return 0.0;
    }
    int previous_value = 0;
    auto it = permutation.begin();
    auto end = permutation.end();
    for(;it != end;++it){
        cost += config.dist[previous_value*config.nbVertex+*it];
        previous_value = *it;
    }
    cost += config.dist[previous_value];
    return cost;
}

// Fonction pour générer des solutions voisines
std::vector<TabouSolution> generateNeighbors(const TabouSolution &currentSolution, Config &config) {
    std::vector<TabouSolution> neighbors;
    for (size_t i = 0; i < currentSolution.tour.size() - 1; ++i) {
        for (size_t j = i + 1; j < currentSolution.tour.size(); ++j) {
            TabouSolution neighbor = currentSolution;
            std::swap(neighbor.tour[i], neighbor.tour[j]); // Échanger deux clients
            neighbor.cost = evaluate_permutation_tabou(config, neighbor.tour); // Évaluer le coût de la nouvelle solution
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

// Fonction de recherche tabou
TabouSolution tabouSearch(Config &config, const TabouSolution &initialSolution, int maxIterations, int tabouListSize) {
    std::set<std::vector<int>> tabouList;
    TabouSolution bestSolution = initialSolution;
    TabouSolution currentSolution = initialSolution;

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        auto neighbors = generateNeighbors(currentSolution, config);
        TabouSolution bestNeighbor;
        bool found = false;

        for (const auto &neighbor: neighbors) {
            if (tabouList.find(neighbor.tour) == tabouList.end() ||
                neighbor.cost < bestSolution.cost) { // Si pas dans la liste tabou ou meilleur que la meilleure solution
                if (!found || neighbor.cost < bestNeighbor.cost) {
                    bestNeighbor = neighbor;
                    found = true;
                }
            }
        }

        if (found) {
            if (bestNeighbor.cost < bestSolution.cost) {
                bestSolution = bestNeighbor; // Mise à jour de la meilleure solution
            }
            tabouList.insert(currentSolution.tour); // Ajouter la solution actuelle à la liste tabou
            currentSolution = bestNeighbor; // Passer à la meilleure solution voisine

            // Maintenir la taille de la liste tabou
            if (tabouList.size() > tabouListSize) {
                tabouList.erase(tabouList.begin()); // Supprimer l'élément le plus ancien
            }
        } else {
            break; // Aucun voisin valide trouvé, sortir de la boucle
        }
    }
    return bestSolution;
}

TSPResults fill_results_rech_tabou(Config &config, int verbose) {
    // Définir une solution initiale (à adapter selon vos besoins)
    std::vector<int> initialPermutation(config.nbVertex);
    std::iota(initialPermutation.begin(), initialPermutation.end(), 0); // Initiale séquentielle
    std::cout << "Initial permutation: " << initialPermutation << std::endl;

    // Convertir la solution initiale en TabouSolution avec coût évalué
    TabouSolution initialSolution{initialPermutation, evaluate_permutation_tabou(config, initialPermutation)};
    std::cout << "Initial solution cost: " << initialSolution.cost << std::endl;
    std::cout << "Initial solution tour: " << initialSolution.tour << std::endl;

    // Paramètres pour la recherche tabou
    int maxIterations = 1000;
    int tabouListSize = 10;

    // Exécuter la recherche tabou
    TabouSolution bestSolution = tabouSearch(config, initialSolution, maxIterations, tabouListSize);

    // Préparer les TSPResults
    TSPResults results(config.nbVertex);
    for (int i = 0; i < config.nbVertex; ++i) {
        // Supposer que l'indice i correspond à un sous-ensemble spécifique des nœuds
        // et que vous voulez le coût pour ce sous-ensemble.
        results[i] = evaluate_permutation_tabou(config,
                                                bestSolution.tour); // Ou utiliser un sous-ensemble spécifique si nécessaire
    }

    // Affichage des informations si verbose est activé
    if (verbose > 0) {
        std::cout << "Best Tabou Solution Cost: " << bestSolution.cost << std::endl;
    }
    return results;
}

float exact_solver_tabou(Config& config, int verbose){
    TSPResults results = fill_results_rech_tabou(config, verbose);
    return solve_partitionning_problem(config, results);
}