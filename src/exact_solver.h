#pragma once
#include <vector>
#include <array>


using Partition = std::vector<int>;
using Capacities = std::vector<float>;
using ListOfNodes = std::vector<int>;

using Score_ptr = std::shared_ptr<float>;

using TSPResults = std::vector<float>;
using HKResults = std::vector<float>;



/**
 * @brief Compute the Held-Karp algorithm and fill the table for every subset of vertices
 * 
 * @param config The configuration of the problem
 * @param verbose The verbose level between 0 and 2
 * @return TSPResults The TSP solution for every subset of vertices
 */
TSPResults fill_results_held_karp(Config& config, int verbose);


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
void solve_partitionning_problem_rec(Config& config, TSPResults& results, Partition partition, int vertex_num, int vertex_num_pow, Capacities capacities, Score_ptr best_score);

/**
 * @brief Solve the partitionning problem by brute force
 * 
 * @param config The configuration of the problem
 * @param results The TSP solution for every subset of vertices
 * @return float The best score found
 */
float solve_partitionning_problem(Config& config, TSPResults& results);

/**
 * @brief Launch the exact solver for the problem
 * 
 * @param config The configuration of the problem
 * @param verbose The verbose level between 0 and 2
 * @return float The best score found
 */
float exact_solver(Config& config, int verbose);

