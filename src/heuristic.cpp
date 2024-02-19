#pragma once
#include "heuristic.h"
#include "config.h"
#include "exact_solver.h"
#include "exact_solver.cpp"
#include <bits/stdc++.h>
#include <iostream>
#include <algorithm>


float kruskalMSTCost(Config& config, Edges edges, int subsetSize) 
{ 
    float mst_wt = 0.0; // Initialize result 
  
    // Sort edges in increasing order on basis of cost 
    sort(edges.begin(), edges.end());

    // Create disjoint sets 
    DisjointSets ds(subsetSize);

    // Iterate through all sorted edges 
    Edges::iterator it; 
    for (it=edges.begin(); it!=edges.end(); it++) 
    { 
        int u = it->second.first; 
        int v = it->second.second; 
  
        int set_u = ds.find(u);
        int set_v = ds.find(v);
  
        // Check if the selected edge is creating 
        // a cycle or not (Cycle is created if u 
        // and v belong to same set) 
        if (set_u != set_v) 
        { 
            // Current edge will be in the MST 
            // so print it 
            // std::cout << u << " - " << v << std::endl; 
  
            // Update MST weight 
            mst_wt += it->first; 
  
            // Merge two sets 
            ds.merge(set_u, set_v); 
        } 
    }
    return mst_wt;
} 


float compute_upper_bound(Config& config, ListOfNodes subset, Edges edges){
    return 2.0*kruskalMSTCost(config, edges, subset.size());
}

float compute_lower_bound(Config& config, ListOfNodes subset){
    float max = 0.0;
    int n = subset.size();
    if(n == 1){
        return 0.0;
    }
    if(n == 2){
        return 2.0*config.dist[subset[0]*config.nbVertex+subset[1]];
    }
    for(int k=0; k<n; k++){
        Edges edges;
        for(int i=0; i<n-1; i++){
            if(i != k){
                for(int j=i+1; j<n; j++){
                    if(j != k){
                        edges.push_back(std::make_pair(config.dist[subset[i]*config.nbVertex+subset[j]], std::make_pair(i, j)));
                    }
                }
            }
        }
        float mst_wt = kruskalMSTCost(config, edges, n-1);
        float min1 = 10000000.0;
        float min2 = 10000000.0;
        for(int i=0; i<n; i++){
            if(i != k){
                if(config.dist[subset[i]*config.nbVertex+subset[k]] < min1){
                    min2 = min1;
                    min1 = config.dist[subset[i]*config.nbVertex+subset[k]];
                }
                else if(config.dist[subset[i]*config.nbVertex+subset[k]] < min2){
                    min2 = config.dist[subset[i]*config.nbVertex+subset[k]];
                }
            }
        }
        if (min1 + min2 + mst_wt > max){
            max = min1 + min2 + mst_wt;
        }
    }
    return max;
}

float compute_new_distance(Config& config, ListOfNodes subset){
    Edges edges;
    float bound = 0.0;
    int n0 = subset.size();
    if(n0< 13){
        bound = get_one_TSP_result(config, subset);
    }
    else{
        subset.push_back(0);
        int n = subset.size();
        for(int i=0; i<n-1; i++){
            for(int j=i+1; j<n; j++){
                edges.push_back(std::make_pair(config.dist[subset[i]*config.nbVertex+subset[j]], std::make_pair(i,j)));
            }
        }
        bound = compute_upper_bound(config, subset, edges);
    }
    // float lower_bound = compute_lower_bound(config, subset);
    // float upper_bound = compute_upper_bound(config, subset, edges);
    // std::cout << lower_bound << "\t" << exact << "\t" << upper_bound << "\t" << subset.size() << std::endl;
    // std::cout << subset << std::endl;
    
    return bound;
}

float get_total_score(Config& config, BigPartition partition,Distances distances){
    float cost = 0.0;
    // For long term vehicles
    for(int i=0; i<config.nbVehicle; i++){
        if (partition[i].size() == 0){
            // is empty
        }
        else{
            float distance = distances[i];
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
        if (partition[i].size() == 0){
            // is empty
        }
        else{
            cost += config.fixedCostShortTermVehicle[i-config.nbVehicle];
        }
    }
    return cost;

}

bool allowed_subset(Config& config, float capacity, ListOfNodes subset, int vehicle, int vertex_num, float distance){
    if (vehicle >= config.nbVehicle){
        if(subset.size() == 1){
            int numSTV = vehicle - config.nbVehicle;
            if(config.HardDistanceLimitShortTermVehicle[numSTV] > config.dist[vertex_num]){
                return true;
            }
            return false;
        }
        return false;
    }
    if(config.Capacity[vehicle] < capacity){
        return false;
    }

    float time = distance / config.speed[vehicle];
    if(time > config.HardTimeLimit[vehicle]){
        return false;
    }
    return true;
}

void partition_search_rec(Config config, BigPartition partition, int vertex, int place, Capacities capacities, Distances distances, Score_ptr best_score, std::shared_ptr<float> best_height, int verbose=0){
    partition[place].push_back(vertex);
    capacities[place] += config.Demand[vertex];
    float new_distance = compute_new_distance(config,partition[place]);
    distances[place] = new_distance;
    if(vertex < *best_height){
        *best_height = vertex;
        if(verbose > 1){
            std::cout << "height: " << vertex<< std::endl;
        }
        
    }
    if (allowed_subset(config, capacities[place], partition[place], place, vertex, new_distance)){
        float score = get_total_score(config, partition, distances);
        if(vertex == 1){
            if(score < *best_score){
                *best_score = score;
                if(verbose > 0){

                    std::cout << "New best score: " << score << std::endl;
                    if (verbose > 1){
                        std::cout << "New best partition: " << std::endl;
                        std::cout << partition << std::endl;
                        std::cout << "Capacities: " << capacities << std::endl;
                        std::cout << "Distances: " << distances << std::endl;
                    }
                }
            }
            return;
        }
        std::vector<int> long_term_vehicles;
        for(int i=config.nbVehicle-1; i>=0; i--){
            long_term_vehicles.push_back(i);
        }
        // std::shuffle(long_term_vehicles.begin(), long_term_vehicles.end(), std::default_random_engine(0));
        for(auto i: long_term_vehicles){
            partition_search_rec(config, partition, vertex-1, i, capacities, distances, best_score,best_height, verbose);
        }
        
        for(int i=config.nbVehicle; i<config.nbVehicle + config.nbShortTermVehicle; i++){
            partition_search_rec(config, partition, vertex-1, i, capacities, distances, best_score,best_height, verbose);
        }
        
        
        
    }
}


float partition_search(Config& config, int verbose){
    int nbTotalVehicle = config.nbVehicle + config.nbShortTermVehicle;
    BigPartition partition(nbTotalVehicle);
    // initialize capacities with 0
    std::vector<float> capacities(nbTotalVehicle, 0.0);
    std::vector<float> distances(nbTotalVehicle, 0.0);
    std::shared_ptr<float> best_score = std::make_shared<float>(100000000);
    std::shared_ptr<float> best_height = std::make_shared<float>(10000000);
    std::vector<int> long_term_vehicles;
    for(int i=config.nbVehicle-1; i>=0; i--){
        long_term_vehicles.push_back(i);
    }
    // std::shuffle(long_term_vehicles.begin(), long_term_vehicles.end(), std::default_random_engine(0));
    for(auto i: long_term_vehicles){
        partition_search_rec(config, partition, config.nbVertex-1, i, capacities, distances, best_score,best_height, verbose);
    }
    
    for(int i=config.nbVehicle; i<config.nbVehicle + config.nbShortTermVehicle; i++){
        partition_search_rec(config, partition, config.nbVertex-1, i, capacities, distances, best_score,best_height, verbose);
    }
    return *best_score;
}

float heuristic_solver(Config& config, int verbose){
    return partition_search(config, verbose);
}