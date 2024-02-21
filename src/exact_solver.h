#pragma once
#include <vector>
#include <array>
#include <memory>
#include "config.h"
#include "vrp.h"


using Partition = std::vector<int>;
using Capacities = std::vector<float>;
using ListOfNodes = std::vector<int>;
using Permutation = std::vector<int>;

using Score_ptr = std::shared_ptr<float>;
using Partition_ptr = std::shared_ptr<Partition>;

using TSPResults = std::vector<float>;
using HKResults = std::vector<float>;
using HKPredecessors = std::vector<int>;
using Path = std::vector<int>;


/**
 * @brief Evaluate the score of a permutation of the nodes
 * 
 * @param config The configuration of the problem
 * @param permutation The permutation of the nodes
 * @return float The total distance of the permutation
 */
float evaluate_permutation(Config& config, Permutation permutation);

/**
 * @brief go through all the permutations of the nodes
 * 
 * @param config The configuration of the problem
 * @param nodes The list of nodes
 * @return float The best total distance found
 */
float solve_TSP_brute_force(Config& config, ListOfNodes nodes);

/**
 * @brief Switch from the power of two representation to the list of nodes
 * 
 * @param n The sum of the powers of two to decompose
 * @return ListOfNodes The power of two decomposition
 */
ListOfNodes power_of_two_decomposition(int n);

/**
 * @brief Say whether the total demand is below the capacity of the biggest vehicle (useful to compute or not)
 * 
 * @param config The configuration of the problem
 * @param nodes The list of nodes
 * @return bool
 */
bool in_range_of_highest_capacity(Config& config, ListOfNodes nodes);

/**
 * @brief Brute force the TSP problem for every subset of vertices
 * 
 * @param config The configuration of the problem
 * @param verbose The verbose level between 0 and 2
 * @return TSPResults The TSP solution for every subset of vertices
 */
TSPResults fill_results_brute_force(Config& config, int verbose);

/**
 * @brief Auxiliary function to compute the Held-Karp algorithm
 * 
 * @param config The configuration of the problem
 * @param hk_results The results of the Held-Karp algorithm to be filled
 * @param i The final vertex
 * @param set The set of vertices to visit
 * @return float The shortest way to visit the set of vertices and end at i
 */
float compute_held_karp_rec(Config& config, HKResults& hk_results, HKPredecessors& hk_pred, int i, int set);

/**
 * @brief Compute the Held-Karp algorithm with dynamic programming
 * 
 * @param config The configuration of the problem
 * @param hk_results The results of the Held-Karp algorithm
 */
void compute_held_karp(Config& config, HKResults& hk_results, HKPredecessors& hk_pred);

/**
 * @brief Compute the Held-Karp algorithm and fill the table for every subset of vertices
 * 
 * @param config The configuration of the problem
 * @param verbose The verbose level between 0 and 2
 * @return TSPResults The TSP solution for every subset of vertices
 */
TSPResults fill_results_held_karp(Config& config, int verbose);

/**
 * @brief Compute the score of a partition
 * 
 * @param config The configuration of the problem
 * @param results The TSP solution for every subset of vertices
 * @param partition For every vehicle, the list of vertices it visits (in for of a sum of powers of 2)
 * @return float The score of the partition
 */
float get_partition_score(Config& config, TSPResults results, Partition partition);

/**
 * @brief Say whether a partition so far is allowed or not, by checking the capacity of every vehicle and the time taken
 * 
 * @param config The configuration of the problem
 * @param results The TSP solution for every subset of vertices
 * @param partition For every vehicle, the list of vertices it visits (in for of a sum of powers of 2)
 * @param vehicle The vehicle we are currently looking at
 * @param vertex_num The number of the vertex we are currently looking at
 * @param vertex_num_pow The power of 2 of the vertex we are currently looking at
 * @param capacities The amount of capacity used by every vehicle
 * @return true If the partition is allowed
 */
bool allowed_partition(Config& config, TSPResults& results, Partition partition, int vehicle, int vertex_num, int vertex_num_pow, Capacities capacities);


/**
 * @brief Auxiliary function to brute force the partitionning problem
 * 
 * @param config The configuration of the problem
 * @param results The TSP solution for every subset of vertices
 * @param partition For every vehicle, the list of vertices it visits (in for of a sum of powers of 2)
 * @param vertex_num The number of the vertex we are currently looking at
 * @param vertex_num_pow The power of 2 of the vertex we are currently looking at
 * @param capacities The amount of capacity used by every vehicle
 * @param best_score The best score found so far
 */
void solve_partitionning_problem_rec(Config& config, TSPResults& results, Partition partition, int vertex_num, int vertex_num_pow, Capacities capacities, Score_ptr best_score, Partition_ptr best_partition);

/**
 * @brief Solve the partitionning problem by brute force
 * 
 * @param config The configuration of the problem
 * @param results The TSP solution for every subset of vertices
 * @return float The best score found
 */
Partition solve_partitionning_problem(Config& config, TSPResults& results);

/**
 * @brief Launch the exact solver for the problem
 * 
 * @param config The configuration of the problem
 * @param verbose The verbose level between 0 and 2
 * @return float The best score found
 */
Solution exact_solver(Config& config, int verbose);

