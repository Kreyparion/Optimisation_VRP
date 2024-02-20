#pragma once

#include "config.h"
#include <vector>
#include <list>
#include <algorithm>
#include <limits>

class TabouSearch {
public:
    explicit TabouSearch(Config config, int num_iterations = 1000, int tabou_tenure = 10, bool verbose = false);
    float run();

private:
    Config config;
    std::vector<std::vector<int>> currentSolution;
    std::vector<std::vector<int>> bestSolution;
    std::list<std::pair<int, int>> tabouList; // Pair of vertex indices representing a move
    float bestCost = std::numeric_limits<float>::max();
    int num_iterations;
    int tabou_tenure;
    bool verbose;

    std::vector<std::vector<int>> generateInitialSolution();
    float calculateCost(const std::vector<std::vector<int>>& solution);
    std::vector<std::pair<int, int>> generateCandidateMoves();
    void displaySolution(const std::vector<std::vector<int>>& solution) const;
    bool canAssignLongTerm(const std::vector<int>& tour, int client, int vehicle);
    bool canAssignShortTerm(int client, int vehicle);
    void applyMove(std::pair<int, int> move);
    bool isTabou(const std::pair<int, int>& move);
    void updateTabouList(const std::pair<int, int>& move);
    void decrementTabouTenure();
};