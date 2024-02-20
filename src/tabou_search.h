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
    vector<vector<int>> currentSolution;
    vector<vector<int>> bestSolution;
    list<pair<int, int>> tabouList; // Pair of vertex indices representing a move
    float bestCost = numeric_limits<float>::max();
    int num_iterations;
    int tabou_tenure;
    bool verbose;

    vector<vector<int>> generateInitialSolution();
    float calculateCost(const vector<vector<int>>& solution);
    vector<pair<pair<int, int>, pair<int, int>>> generateCandidateMoves();
    void displaySolution(const vector<vector<int>>& solution) const;
    bool canAssignLongTerm(const vector<int>& tour, int client, int vehicle);
    bool canAssignShortTerm(const vector<int> &tour, int client, int vehicle);
    bool canAssignClient(vector<int> &vector, int client, int vehicule);
    void applyMove(pair<int, int> move);
    bool isValidMove(int client, int fromVehicle, int toVehicle);
    bool isTabou(const pair<int, int>& move);
    void updateTabouList(const pair<int, int>& move);
    void decrementTabouTenure();

};