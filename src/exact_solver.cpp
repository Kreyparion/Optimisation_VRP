#pragma once
#include <vector>
#include <iostream>
#include <array>
#include <algorithm>
#include <math.h>
#include <memory>
#include <omp.h>


#include "config.h"
#include "exact_solver.h"

float evaluate_permutation(Config& config, Permutation permutation){
    float cost = 0.0;
    if (permutation.size() == 0){
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

float solve_TSP_brute_force(Config& config, ListOfNodes nodes){
    float best_cost = 10000000;
    // go through all the permutations of the nodes
    std::vector<int> permutation = nodes;
    do {
        float cost = evaluate_permutation(config,permutation);
        if(cost < best_cost){
            best_cost = cost;
        }
    }
    while (std::next_permutation(permutation.begin(),permutation.end()));
    return best_cost;
}


ListOfNodes power_of_two_decomposition(int n){
    std::vector<int> nodes;
    for(int i=0; i<32; i++){
        if(n & (1 << i)){
            nodes.push_back(i+1);
        }
    }
    return nodes;
}

bool in_range_of_highest_capacity(Config& config, ListOfNodes nodes){
    float demand = 0.0;
    int n = nodes.size();
    for(int i=0; i<n; i++){
        demand += config.Demand[nodes[i]];
    }
    if(demand > config.Capacity[0]){
        return false;
    }
    return true;
}

TSPResults fill_results_brute_force(Config& config, int verbose=0){
    int nbIter = std::pow(2, config.nbVertex-1);
    TSPResults results(nbIter, 0.0);
    int nb_computed = 0;
    #pragma omp parallel shared(results,nb_computed) num_threads(8)
    {
        #pragma omp for nowait
        for(int i=0; i<nbIter; i++){
            std::vector<int> nodes = power_of_two_decomposition(i);
            if (in_range_of_highest_capacity(config, nodes)){
                float result = solve_TSP_brute_force(config, nodes);
                results[i] = result;
            }
            nb_computed += 1;
            if (nb_computed % 1000 == 0 and verbose >= 1){
                std::cout << "i: " << nb_computed << " / " << nbIter << " done" << std::endl;
            }
        }
    }
    return results;
}

float compute_held_karp_rec(Config& config, HKResults& hk_results, int i, int set){
    int nbIter = 1 << (config.nbVertex-1);
    int num = 1 << i;
    int subset = set - num;
    if(hk_results[i*nbIter+subset] != 0.0){
        return hk_results[i*nbIter+subset];
    }
    int pow_j = 1;
    float min = 10000000.0;
    for(int j=0; j<config.nbVertex-1; j++){
        if((subset & pow_j) != 0){
            float value = compute_held_karp_rec(config, hk_results, j, subset);
            float distance = value + config.dist[(i+1)*config.nbVertex+j+1];
            if(distance < min){
                min = distance;
            }
        }
        pow_j = pow_j << 1;
    }
    hk_results[i*nbIter+subset] = min;
    return min;
}


void compute_held_karp(Config& config, HKResults& hk_results){
    int nbIter = 1 << (config.nbVertex-1);
    for(int i=0; i<config.nbVertex-1; i++){
        hk_results[i*nbIter] = config.dist[i+1];
    }
    int max = nbIter - 1;
    for(int i=0; i<config.nbVertex-1; i++){
        compute_held_karp_rec(config, hk_results, i, max);
    }
}

TSPResults fill_results_held_karp(Config& config, int verbose=0){
    int nbIter = 1 << (config.nbVertex-1);
    TSPResults results(nbIter, 0.0);
    HKResults hk_results((config.nbVertex-1)*nbIter, 0.0);
    compute_held_karp(config, hk_results);
    
    for(int i=1; i<nbIter; i++){
        float min = 10000000.0;
        int pow_j = 1;
        for(int j=0; j<config.nbVertex-1; j++){
            if((i & pow_j) != 0){
                float distance = hk_results[j*nbIter+i-pow_j] + config.dist[0*config.nbVertex+j+1];
                if(distance < min){
                    min = distance;
                }
            }
            pow_j = pow_j << 1;
        }
        results[i] = min;
    }
    return results;
}

/** Adel **/
// Structure pour une solution
struct TabouSolution {
    std::vector<int> tour; // Représentation d'une solution (e.g., ordre de visite des clients)
    float cost{}; // Coût de la solution
};

// Fonction pour générer des solutions voisines
std::vector<TabouSolution> generateNeighbors(const TabouSolution &currentSolution, Config &config) {
    std::vector<TabouSolution> neighbors;
    for (size_t i = 0; i < currentSolution.tour.size() - 1; ++i) {
        for (size_t j = i + 1; j < currentSolution.tour.size(); ++j) {
            TabouSolution neighbor = currentSolution;
            std::swap(neighbor.tour[i], neighbor.tour[j]); // Échanger deux clients
            neighbor.cost = evaluate_permutation(config, neighbor.tour); // Évaluer le coût de la nouvelle solution
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
    cout << "Initial permutation: " << initialPermutation << endl;

    // Convertir la solution initiale en TabouSolution avec coût évalué
    TabouSolution initialSolution{initialPermutation, evaluate_permutation(config, initialPermutation)};
    cout << "Initial solution cost: " << initialSolution.cost << endl;
    cout << "Initial solution tour: " << initialSolution.tour << endl;

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
        results[i] = evaluate_permutation(config,
                                          bestSolution.tour); // Ou utiliser un sous-ensemble spécifique si nécessaire
    }

    // Affichage des informations si verbose est activé
    if (verbose > 0) {
        std::cout << "Best Tabou Solution Cost: " << bestSolution.cost << std::endl;
    }
    return results;
}

float get_partition_score(Config& config, TSPResults results, Partition partition){
    float cost = 0.0;
    // For long term vehicles
    for(int i=0; i<config.nbVehicle; i++){
        if (partition[i] == 0){
            // is empty
        }
        else{
            float distance = results[partition[i]];
            if (distance > config.SoftDistanceLimit[i]){
                cost += config.distancePenalty[i] * (distance - config.SoftDistanceLimit[i]);
            }
            float time = distance / config.speed[i];
            if (time > config.SoftTimeLimit[i]){
                cost += config.timePenalty[i] * (time - config.SoftTimeLimit[i]);
            }
            cost += config.fixedCostVehicle[i];
        }
    }
    // For short term vehicles
    for(int i=config.nbVehicle; i<config.nbVehicle+config.nbShortTermVehicle; i++){
        if (partition[i] == 0){
            // is empty
        }
        else{
            cost += config.fixedCostShortTermVehicle[i-config.nbVehicle];
        }
    }
    return cost;

}



bool allowed_partition(Config& config, TSPResults& results, Partition partition, int vehicle, int vertex_num, int vertex_num_pow, Capacities capacities){
    if (vehicle >= config.nbVehicle){
        if(partition[vehicle] == 0){
            int numSTV = vehicle - config.nbVehicle;
            if(config.HardDistanceLimitShortTermVehicle[numSTV] > config.dist[vertex_num]){
                return true;
            }
            return false;
        }
        return false;
    }
    if(config.Capacity[vehicle] < capacities[vehicle] + config.Demand[vertex_num]){
        return false;
    }
    float new_distance = results[vertex_num_pow+partition[vehicle]];

    float time = new_distance / config.speed[vehicle];
    if(time > config.HardTimeLimit[vehicle]){
        return false;
    }
    return true;

}


void solve_partitionning_problem_rec(Config& config, TSPResults& results, Partition partition, int vertex_num, int vertex_num_pow, Capacities capacities, Score_ptr best_score){
    if(vertex_num <= 0){
        float score = get_partition_score(config, results, partition);
        if(score < *best_score){
            *best_score = score;
        }
    }
    else{
        int nbTotalVehicle = config.nbVehicle + config.nbShortTermVehicle;
        for(int i=0; i<nbTotalVehicle; i++){
            if (allowed_partition(config, results, partition, i, vertex_num, vertex_num_pow, capacities)){
                partition[i] += vertex_num_pow;
                capacities[i] += config.Demand[vertex_num];
                solve_partitionning_problem_rec(config, results, partition, vertex_num-1, vertex_num_pow >> 1, capacities, best_score);
                capacities[i] -= config.Demand[vertex_num];
                partition[i] -= vertex_num_pow;
            }
            
        }
    }
}


float solve_partitionning_problem(Config& config, TSPResults& results){
    int nbTotalVehicle = config.nbVehicle + config.nbShortTermVehicle;
    std::vector<int> partition;
    for(int i=0; i<nbTotalVehicle; i++){
        partition.push_back(0);
    }
    int vertex_num_pow = 1 << (config.nbVertex-2);
    // initialize capacities with 0
    std::vector<float> capacities;
    for(int i=0; i<nbTotalVehicle; i++){
        capacities.push_back(0.0);
    }
    std::shared_ptr<float> best_score = std::make_shared<float>(10000000);
    solve_partitionning_problem_rec(config, results, partition, config.nbVertex-1, vertex_num_pow, capacities, best_score);
    return *best_score;
}


float exact_solver(Config& config, int verbose=0){
    TSPResults results = fill_results_held_karp(config, verbose);
    return solve_partitionning_problem(config, results);
}
