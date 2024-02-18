#pragma once
#include <vector>
#include <string>
#include <iostream>

// display where the config is from (csv)
/**
 * @brief The configuration of the problem with all the parameters
 *
 * @param nbVehicle The total number of long term vehicles
 * @param vehicleCounts The number of vehicles of each type
 * @param nbVertex The number of vertices
 * @param nbShortTermVehicle The total number of short term vehicles
 * @param shortTermVehicleCounts The number of short term vehicles of each type
 * @param dist The distance matrix
 * @param speed The speed of each vehicle
 * @param fixedCostShortTermVehicle The fixed cost of each short term vehicle
 * @param fixedCostVehicle The fixed cost of each long term vehicle
 * @param timePenalty The time penalty of each vehicle
 * @param distancePenalty The distance penalty of each vehicle
 * @param HardTimeLimit The hard time limit of each vehicle
 * @param SoftTimeLimit The soft time limit of each vehicle
 * @param SoftDistanceLimit The soft distance limit of each vehicle
 * @param HardDistanceLimitShortTermVehicle The hard distance limit of each short term vehicle
 * @param Capacity The capacity of each vehicle
 * @param Demand The demand of each vertex
 */
class Config{
    public:
        int nbVehicle;
        std::vector<float> vehicleCounts;
        int nbVertex;
        int nbShortTermVehicle;
        std::vector<float> shortTermVehicleCounts;
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

//  display the distance matrix
void display_dist(std::ostream& os, std::vector<float> const& dist, int size){
    for (int i = 0; i < size; i++){
        os << "\t";
        for(int j = 0; j < size; j++){
            os << dist[i*size + j] << "\t";
        }
        os << std::endl;
    }
};

// display the config
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


/**
 * @brief Import the data from a csv file
 *
 * @param num The number of the file (between 1 and 17)
 * @param verbose The verbose level between 0 and 2
 * @return Config The configuration of the problem
 */
Config import_data(int num, bool verbose);


/**
 * @brief Extend the configuration with side effect : for every vehicle type, add it as many times as there are vehicles of this type
 *
 * @param config The configuration of the problem
 */
void extend_config(Config& config);


/**
 * @brief Get the configuration of the problem (data from the csv file)
 *
 * @param num The number of the file (between 1 and 17)
 * @param verbose The verbose level between 0 and 2
 * @return Config The configuration of the problem
 */
Config getConfig(int num, bool verbose);