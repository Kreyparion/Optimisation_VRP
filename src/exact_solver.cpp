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

float compute_held_karp_rec(Config& config, HKResults& hk_results, HKPredecessors& hk_pred, int i, int set, ListOfNodes nodes){
    int n = nodes.size();
    int nbIter = 1 << n;
    int num = 1 << i;
    int subset = set - num;
    if(hk_results[i*nbIter+subset] != 0.0){
        return hk_results[i*nbIter+subset];
    }
    int pow_j = 1;
    float min = 10000000.0;
    int min_j = 0;
    for(int j=0; j<n; j++){
        if((subset & pow_j) != 0){
            float value = compute_held_karp_rec(config, hk_results, hk_pred, j, subset, nodes);
            float distance = value + config.dist[nodes[i]*config.nbVertex+nodes[j]];
            if(distance < min){
                min = distance;
                min_j = j;
            }
        }
        pow_j = pow_j << 1;
    }
    hk_results[i*nbIter+subset] = min;
    if(hk_pred.size() > 0){
        hk_pred[i*nbIter+subset] = min_j;
    }
    return min;
}


void compute_held_karp(Config& config, HKResults& hk_results, HKPredecessors& hk_pred, ListOfNodes nodes){
    int n = nodes.size();
    int nbIter = 1 << n;
    for(int i=0; i<n; i++){
        hk_results[i*nbIter] = config.dist[nodes[i]];
    }
    int max = nbIter - 1;
    for(int i=0; i<n; i++){
        compute_held_karp_rec(config, hk_results, hk_pred, i, max, nodes);
    }
}

float get_one_TSP_result(Config& config, ListOfNodes nodes){
    int n = nodes.size();
    if (n <= 0){
        return 0.0;
    }
    int nbIter = 1 << n;
    HKResults hk_results(n*nbIter, 0.0);
    HKPredecessors hk_pred;
    compute_held_karp(config, hk_results, hk_pred, nodes);
    float min = 10000000.0;
    for(int i=0; i<n; i++){
        int i_power = 1 << i;
        float distance = hk_results[i*nbIter+nbIter-i_power-1] + config.dist[0*config.nbVertex+nodes[i]];
        if(distance < min){
            min = distance;
        }
    }
    return min;
}

Path get_one_TSP_solution(Config& config, ListOfNodes nodes){
    int n = nodes.size();
    if (n <= 0){
        return Path();
    }
    int nbIter = 1 << n;
    HKResults hk_results(n*nbIter, 0.0);
    HKPredecessors hk_pred(n*nbIter, 0);
    compute_held_karp(config, hk_results, hk_pred, nodes);
    float min = 10000000.0;
    int min_i = 0;
    for(int i=0; i<n; i++){
        int i_power = 1 << i;
        float distance = hk_results[i*nbIter+nbIter-i_power-1] + config.dist[0*config.nbVertex+nodes[i]];
        if(distance < min){
            min = distance;
            min_i = i;
        }
    }
    Path path;
    int i = min_i;
    int set = nbIter-1-(1 << i);
    while(set > 0){
        path.push_back(nodes[i]);
        int j = hk_pred[i*nbIter+set];
        set = set - (1 << j);
        i = j;
    }
    path.push_back(nodes[i]);
    return path;
}


Solution return_solution_from_partition(Config& config, Partition partition){
    Solution sol = init_solution(config.nbVehicle+config.nbShortTermVehicle);
    for(int i=0; i<config.nbVehicle; i++){
        sol[i] = get_one_TSP_solution(config, power_of_two_decomposition(partition[i]));
    }
    for(int i=config.nbVehicle; i<config.nbVehicle+config.nbShortTermVehicle; i++){
        sol[i] = power_of_two_decomposition(partition[i]);
    }
    return sol;
}
TSPResults fill_results_held_karp(Config& config, int verbose=0){
    int nbIter = 1 << (config.nbVertex-1);
    TSPResults results(nbIter, 0.0);
    HKResults hk_results((config.nbVertex-1)*nbIter, 0.0);
    HKPredecessors hk_pred;
    ListOfNodes nodes;
    for(int i=1; i<config.nbVertex; i++){
        nodes.push_back(i);
    }
    compute_held_karp(config, hk_results, hk_pred, nodes);
    
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


void solve_partitionning_problem_rec(Config& config, TSPResults& results, Partition partition, int vertex_num, int vertex_num_pow, Capacities capacities, Score_ptr best_score, Partition_ptr best_partition){
    if(vertex_num <= 0){
        float score = get_partition_score(config, results, partition);
        if(score < *best_score){
            *best_score = score;
            *best_partition = partition;
        }
    }
    else{
        int nbTotalVehicle = config.nbVehicle + config.nbShortTermVehicle;
        for(int i=0; i<nbTotalVehicle; i++){
            if (allowed_partition(config, results, partition, i, vertex_num, vertex_num_pow, capacities)){
                partition[i] += vertex_num_pow;
                capacities[i] += config.Demand[vertex_num];
                solve_partitionning_problem_rec(config, results, partition, vertex_num-1, vertex_num_pow >> 1, capacities, best_score, best_partition);
                capacities[i] -= config.Demand[vertex_num];
                partition[i] -= vertex_num_pow;
            }
            
        }
    }
}


Partition solve_partitionning_problem(Config& config, TSPResults& results){
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
    std::shared_ptr<Partition> best_partition = std::make_shared<Partition>(partition);
    solve_partitionning_problem_rec(config, results, partition, config.nbVertex-1, vertex_num_pow, capacities, best_score, best_partition);
    return *best_partition;
}


Solution exact_solver(Config& config, int verbose=0){
    TSPResults results = fill_results_held_karp(config, verbose);
    Partition best_partition = solve_partitionning_problem(config, results);
    return return_solution_from_partition(config, best_partition);
}
