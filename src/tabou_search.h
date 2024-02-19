#pragma once

#include "config.h"
#include <vector>
#include <list>
#include <algorithm>
#include <limits>

class TabouSearch {
public:
    explicit TabouSearch(const Config& config);
    float run();

private:
    Config config;
    std::vector<std::vector<int>> currentSolution;
    std::vector<std::vector<int>> bestSolution;
    std::list<std::pair<int, int>> tabouList; // Pair of vertex indices representing a move
    float bestCost = std::numeric_limits<float>::max();
    int num_iterations;
    int tabou_tenure;

    std::vector<std::vector<int>> generateInitialSolution();
    float calculateCost(const std::vector<std::vector<int>>& solution);
    std::vector<std::pair<int, int>> generateCandidateMoves();
    static void displaySolution(const std::vector<std::vector<int>>& solution);
    bool canAccommodate(const std::vector<int>& tour, int client, int vehicle);
    void applyMove(std::pair<int, int> move);
    bool isTabou(const std::pair<int, int>& move);
    void updateTabouList(const std::pair<int, int>& move);
    void decrementTabouTenure();
};