#pragma once
#include <vector>
#include <string>

class Config{
    public:
        int nbVehicle;
        int nbVertex;
        int nbShortTermVehicle;
        std::vector<float> dist;
        std::vector<float> speed;
        std::vector<float> fixedCostShortTermVehicle;
        std::vector<float> fixedCostVehicle;
        std::vector<float> timePenalty;
        std::vector<float> distancePenalty;
        std::vector<float> HardTimeLimit;
        std::vector<float> SoftTimeLimit;
        std::vector<float> SoftDistanceLimit;
        std::vector<float> HardDistanceLimitShortTermVehicle;
        std::vector<float> Capacity;
        std::vector<float> Demand;

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
void display_dist(std::ostream& os, std::vector<float> const& dist, int size){
    for (int i = 0; i < size; i++){
        os << "\t";
        for(int j = 0; j < size; j++){
            os << dist[i*size + j] << "\t";
        }
        os << std::endl;
    }
};


std::ostream& operator<<(std::ostream& os, Config const& c){
    os << "NbVertex: " << c.nbVertex << std::endl;

    os << "Dist: ";
    display_dist(os, c.dist, c.nbVertex);
    os << "Demand: " << std::endl;
    os << "\t" << c.Demand << std::endl;
    os << "NbVehicle: " << c.nbVehicle << std::endl;
    os << "NbShortTermVehicle: " << c.nbShortTermVehicle << std::endl;
    os << "Speed: " << std::endl;
    os << "\t\t\t" << c.speed << std::endl;
    os << "FixedCostSTV: " << std::endl;
    os << "\t\t\t" << c.fixedCostShortTermVehicle << std::endl;
    os << "FixedCostVehicle: " << std::endl;
    os << "\t\t\t" << c.fixedCostVehicle << std::endl;
    os << "TimePenalty: " << std::endl;
    os << "\t\t\t" << c.timePenalty << std::endl;
    os << "DistancePenalty: " << std::endl;
    os << "\t\t\t" << c.distancePenalty << std::endl;
    os << "HardTimeLimit: " << std::endl;
    os << "\t\t\t" << c.HardTimeLimit << std::endl;
    os << "SoftTimeLimit: " << std::endl;
    os << "\t\t\t" << c.SoftTimeLimit << std::endl;
    os << "SoftDistanceLimit: " << std::endl;
    os << "\t\t\t" << c.SoftDistanceLimit << std::endl;
    os << "HardDistanceLimitSTV: " << std::endl;
    os << "\t\t\t" << c.HardDistanceLimitShortTermVehicle << std::endl;
    os << "Capacity: " <<  std::endl;
    os << "\t\t\t" << c.Capacity << std::endl;
    
    return os;
};

