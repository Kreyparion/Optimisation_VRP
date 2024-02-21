#pragma once
#include <vector>
#include <iostream>
#include "config.h"


using Solution = std::vector<std::vector<int>>;
using Solution_with_score = std::pair<Solution, float>;

Solution init_solution(int nbTotVehicle){
    Solution sol;
    for(int i=0; i<nbTotVehicle; i++){
        sol.push_back({});
    }
    return sol;
}

// return -1 if the solution is not valid
float compute_and_check_solution(Config& config, Solution& sol){
    float score = 0.0;
    std::vector<bool> visited(config.nbVertex-1, false);
    int n = sol.size();
    if(n != config.nbVehicle + config.nbShortTermVehicle){
        std::cout << "Error: wrong number of vehicles" << std::endl;
        return -1;
    }
    for(int i=0; i<config.nbVehicle; i++){
        int ni = sol[i].size();
        if(ni != 0){
            int capacity = 0;
            float distance = 0.0;
            int last = 0;
            for(int j=0; j<ni; j++){
                int vertex = sol[i][j];
                if(vertex <= 0 || vertex >= config.nbVertex){
                    std::cout << "Error: wrong vertex number" << std::endl;
                    return -1;
                }
                if(visited[vertex-1]){
                    std::cout << "Error: vertex visited twice" << std::endl;
                    return -1;
                }
                visited[vertex-1] = true;
                capacity += config.Demand[vertex];
                distance += config.dist[last*config.nbVertex+vertex];
                last = vertex;
            }
            distance += config.dist[last*config.nbVertex];
            if(capacity > config.Capacity[i]){
                std::cout << "Error: capacity exceeded" << std::endl;
                return -1;
            }
            if(distance > config.SoftDistanceLimit[i]){
                score += config.distancePenalty[i] * (distance - config.SoftDistanceLimit[i]);
            }
            float time = distance / config.speed[i];
            if(time > config.HardTimeLimit[i]){
                std::cout << "Error: time exceeded" << " " << time << " " << config.HardTimeLimit[i] << std::endl;
                return -1;
            }
            if(time > config.SoftTimeLimit[i]){
                score += config.timePenalty[i] * (time - config.SoftTimeLimit[i]);
            }
            score += config.fixedCostVehicle[i];
        }
    }
    for(int i=config.nbVehicle; i<config.nbVehicle+config.nbShortTermVehicle; i++){
        int ni = sol[i].size();
        if(ni != 0){
            if (ni > 1){
                std::cout << "Error: short term vehicle with more than one vertex" << std::endl;
                return -1;
            }
            int vertex = sol[i][0];
            if(vertex <= 0 || vertex >= config.nbVertex){
                std::cout << "Error: wrong vertex number" << std::endl;
                return -1;
            }
            float distance = config.dist[0*config.nbVertex+vertex];
            if(distance > config.HardDistanceLimitShortTermVehicle[i-config.nbVehicle]){
                std::cout << "Error: distance exceeded" << std::endl;
                return -1;
            }
            visited[vertex-1] = true;
            score += config.fixedCostShortTermVehicle[i-config.nbVehicle];
        }
    }
    for(int i=0; i<config.nbVertex-1; i++){
        if(!visited[i]){
            std::cout << "Error: vertex not visited" << std::endl;
            return -1;
        }
    }
    return score;
}