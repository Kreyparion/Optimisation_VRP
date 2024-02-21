#pragma once
#include<bits/stdc++.h> 
#include "exact_solver.h"

// Edge is a cost, a source and a destination
using Edge = std::pair<float, std::pair<int, int>>;
using Edges = std::vector<Edge>;
using ListOfPairs = std::vector<std::pair<int, int>>;
using VisitedList = std::vector<bool>;
using VisitedList_ptr = std::shared_ptr<VisitedList>;
using Path_ptr = std::shared_ptr<Path>;
using int_ptr = std::shared_ptr<int>;
using BigPartition = std::vector<ListOfNodes>;
using BigPartition_ptr = std::shared_ptr<BigPartition>;
using Distances = std::vector<float>;

  
// Create the structure of a graph to compute the MST quickly
struct DisjointSets 
{ 
    int *parent, *rnk; 
    int n; 
  
    // Constructor. 
    DisjointSets(int n) 
    { 
        // Allocate memory 
        this->n = n; 
        parent = new int[n+1]; 
        rnk = new int[n+1]; 
  
        // Initially, all vertices are in 
        // different sets and have rank 0. 
        for (int i = 0; i <= n; i++) 
        { 
            rnk[i] = 0; 
  
            //every element is parent of itself 
            parent[i] = i; 
        } 
    }
    ~DisjointSets(){
        delete[] parent;
        delete[] rnk;
    }
  
    // Find the parent of a node 'u' 
    // Path Compression 
    int find(int u) 
    { 
        /* Make the parent of the nodes in the path 
        from u--> parent[u] point to parent[u] */
        if (u != parent[u]) 
            parent[u] = find(parent[u]); 
        return parent[u]; 
    } 
  
    // Union by rank 
    void merge(int x, int y) 
    { 
        x = find(x), y = find(y); 
  
        /* Make tree with smaller height 
        a subtree of the other tree */
        if (rnk[x] > rnk[y]) 
            parent[y] = x; 
        else // If rnk[x] <= rnk[y] 
            parent[x] = y; 
  
        if (rnk[x] == rnk[y]) 
            rnk[y]++; 
    } 
};

std::ostream& operator<<(std::ostream& os, const Edges& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i].first << "\t" << v[i].second.first << "\t" << v[i].second.second << std::endl;
    }
    return os;
};

std::ostream& operator<<(std::ostream& os, const BigPartition& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << "Partition " << i << std::endl;
        int n = v[i].size();
        for(int j=0; j<n; j++){
            os << v[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
};

std::ostream& operator<<(std::ostream& os, const ListOfPairs& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i].first << "\t" << v[i].second << std::endl;
    }
    return os;
};

std::ostream& operator<<(std::ostream& os, const std::vector<bool>& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i] << " ";
    }
    return os;
};


/**
 * @brief Compute the MST total distance of a graph
 * 
 * @param config The configuration of the problem
 * @param edges The edges of the graph
 * @param subsetSize The size of the subset of the graph
 * @return float The total distance of the MST
 */
float kruskalMSTCost(Config& config, Edges edges, int subsetSize);



/**
 * @brief Compute the MST of a graph, returning the edges of the MST
 * 
 * @param config The configuration of the problem
 * @param edges The edges of the graph
 * @param subsetSize The size of the subset of the graph
 * @return ListOfPairs The List of pairs of vertices of the MST
 */
ListOfPairs kruskalMSTResult(Config& config, Edges edges, int subsetSize);


/**
 * @brief Fill the path recursively with the MST edges
 * 
 * @param config The configuration of the problem
 * @param mst_edges The edges of the MST
 * @param subset The subset of the graph
 * @param vertex The current vertex
 * @param visited The list of visited vertices
 * @param path The path to fill
 * @param nbVisited The number of visited vertices
 */
void create_path_rec(Config& config, ListOfPairs& mst_edges, ListOfNodes& subset, int vertex, VisitedList_ptr visited, Path_ptr path, int_ptr nbVisited);


/**
 * @brief Compute a tour of a graph using the MST and doing shortcuts
 * 
 * @param config The configuration of the problem
 * @param edges The edges of the graph
 * @param subsetSize The size of the subset of the graph
 * @return Path The tour of the graph
 */
Path cleaned_MST(Config& config, ListOfNodes subset, Edges edges);


/**
 * @brief Compute the distance of a path
 * 
 * @param config The configuration of the problem
 * @param path The path to compute the distance
 * @return float The distance of the path
 */
float compute_cost_of_path(Config& config, Path path);


/**
 * @brief Compute an upper bound of the TSP with the MST and the shortcuts
 * 
 * @param config The configuration of the problem
 * @param subset The subset of the graph
 * @param edges The edges of the graph
 * @return float The upper bound of the TSP
 */
float compute_upper_bound(Config& config, ListOfNodes subset, Edges edges);

/**
 * @brief Compute an upper bound of the TSP by multiplying the MST cost by 2
 * 
 * @param config The configuration of the problem
 * @param subset The subset of the graph
 * @param edges The edges of the graph
 * @return float The upper bound of the TSP
 */
float compute_upper_bound2(Config& config, ListOfNodes subset, Edges edges);


/**
 * @brief Compute a lower bound of the TSP with the MST using the Held-Karp lower bound algorithm
 * 
 * @param config The configuration of the problem
 * @param subset The subset of the graph
 * @return float The lower bound of the TSP
 */
float compute_lower_bound(Config& config, ListOfNodes subset);


/**
 * @brief Compute the minimal distance possible of a subset of the graph using an exact solver if possible or an upper bound otherwise
 * 
 * @param config The configuration of the problem
 * @param subset The subset of the graph
 * @return float The minimal distance possible of the subset
 */
float compute_new_distance(Config& config, ListOfNodes subset);


/**
 * @brief Compute an approximated solution to the problem applying the solver on every list of nodes
 * 
 * @param config The configuration of the problem
 * @param partition The partition of the graph
 * @return Solution The approximated solution
 */
Solution return_approximated_solution_from_partition(Config& config, BigPartition partition);


/**
 * @brief Compute cost of a partition
 * 
 * @param config The configuration of the problem
 * @param partition The partition of the graph
 * @return float The cost of the partition
 */
float get_total_score(Config& config, BigPartition partition,Distances distances);

/**
 * @brief Say whether the new subset is allowed in the partition
 * 
 * @param config The configuration of the problem
 * @param capacity The capacity of the vehicle
 * @param subset The subset of the graph
 * @param vehicle The vehicle
 * @param vertex_num The number of vertices
 * @param distance The best distance of the subset
 * @return bool Whether the new subset is allowed in the partition
 */
bool allowed_subset(Config& config, float capacity, ListOfNodes subset, int vehicle, int vertex_num, float distance);



/**
 * @brief Auxiliary function to brute force the partitionning problem while satisfying the constraints, the result is stored in best_partition
 * 
 * @param config The configuration of the problem
 * @param partition The partition of the graph
 * @param vertex_num The number of vertices
 * @param distance The best distance of the subset
 * @param best_score The best score found so far
 * @param best_partition The best partition found so far
 * @param start The start time of the algorithm
 * @param max_time The maximum time allowed for the algorithm
 * @param verbose The verbose level between 0 and 2
 */
void partition_search_rec(Config config, BigPartition partition, int vertex, int place, Capacities capacities, Distances distances, Score_ptr best_score, std::shared_ptr<float> best_height, BigPartition_ptr best_partition, std::chrono::high_resolution_clock::time_point start, int max_time, int verbose);


/**
 * @brief Solve the partitionning problem by brute force
 * 
 * @param config The configuration of the problem
 * @param start The start time of the algorithm
 * @param max_time The maximum time allowed for the algorithm
 * @param verbose The verbose level between 0 and 2
 * @return BigPartition The best partition found
 */
BigPartition partition_search(Config& config, std::chrono::high_resolution_clock::time_point start, int max_time, int verbose);


/**
 * @brief Launch the heuristic solver for the problem by computing a fast algorithm to find the best partition and then solving the TSP for every partition
 * 
 * @param config The configuration of the problem
 * @param max_time The maximum time allowed for the algorithm
 * @param verbose The verbose level between 0 and 2
 * @return Solution The solution found
 */
Solution heuristic_solver(Config& config, int max_time, int verbose);
