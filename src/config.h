#pragma once
#include <vector>
#include <string>

class Config{
    public:
        int nbVehicle;
        int nbVertex; 
        int nbShortTermVehicle;
        std::vector<int> dist;
        std::vector<int> speed;
        std::vector<float> fixedCostShortTermVehicle;
        std::vector<float> fixedCostVehicle;
        std::vector<float> timePenalty;
        std::vector<float> distancePenalty;
        std::vector<float> HardTimeLimit;
        std::vector<float> SoftTimeLimit;
        std::vector<float> SoftDistanceLimit;
        std::vector<float> HardDistanceLimitShortTermVehicle;
        std::vector<int> Capacity;
        std::vector<int> Demand;

};

std::ostream& operator<<(std::ostream& os, std::vector<int> const& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i] << "\t";
    }
    return os;
};

std::ostream& operator<<(std::ostream& os, std::vector<float> const& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i] << "\t";
    }
    return os;
};

//display the distance matrix
void display_dist(std::ostream& os, std::vector<int> const& dist, int size){
    for (int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            os << dist[i*size + j] << "\t";
        }
        os << std::endl;
    }
};


std::ostream& operator<<(std::ostream& os, Config const& c){
    os << "nbVehicle: " << c.nbVehicle << std::endl;
    os << "nbVertex: " << c.nbVertex << std::endl;
    os << "nbShortTermVehicle: " << c.nbShortTermVehicle << std::endl;
    os << "dist: " << std::endl;
    display_dist(os, c.dist, c.nbVertex);
    os << "Demand: " << c.Demand << std::endl;
    os << "speed: " << c.speed << std::endl;
    os << "fixedCostShortTermVehicle: " << c.fixedCostShortTermVehicle << std::endl;
    os << "fixedCostVehicle: " << c.fixedCostVehicle << std::endl;
    os << "timePenalty: " << c.timePenalty << std::endl;
    os << "distancePenalty: " << c.distancePenalty << std::endl;
    os << "HardTimeLimit: " << c.HardTimeLimit << std::endl;
    os << "SoftTimeLimit: " << c.SoftTimeLimit << std::endl;
    os << "SoftDistanceLimit: " << c.SoftDistanceLimit << std::endl;
    os << "HardDistanceLimitSTV: " << c.HardDistanceLimitShortTermVehicle << std::endl;
    os << "Capacity: " << c.Capacity << std::endl;
    
    return os;
};

