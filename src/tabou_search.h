#pragma once

#include "config.h"
#include <vector>
#include <list>
#include <algorithm>
#include <limits>

class TabouSearch {
public:
    explicit TabouSearch(Config config, int num_iterations = 1000, int tabouDuration = 10, bool verbose = false);
    float run();

private:
    Config config;
    vector<vector<int>> currentSolution;
    vector<vector<int>> bestSolution;
    list<pair<pair<pair<int, int>, pair<int, int>>, int>> tabouList;
    float bestCost = numeric_limits<float>::max();
    int num_iterations;
    int tabouDuration;
    int tabouListMaxSize = 10;
    bool verbose;

    vector<vector<int>> generateInitialSolution();
    float calculateCost(const vector<vector<int>>& solution);
    vector<pair<pair<int, int>, pair<int, int>>> generateCandidateMoves(const vector<vector<int>>& solution);
    void displaySolution(const vector<vector<int>>& solution) const;
    bool canAssignLongTerm(const vector<int>& tour, int client, int vehicle);
    bool canAssignShortTerm(const vector<int> &tour, int client, int vehicle);
    void applyMove(pair<pair<int, int>, pair<int, int>> move);
    bool isValidMove(int client, int fromVehicle, int toVehicle);
    bool isTabou(pair<pair<int, int>, pair<int, int>> move);
    void addToTabouList(pair<pair<int, int>, pair<int, int>> move);
    void decrementTabouList();
    void updateTabouList(pair<pair<int, int>, pair<int, int>> move);
};