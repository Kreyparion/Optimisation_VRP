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
