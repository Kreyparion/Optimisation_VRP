#pragma once

#include "config.h"
#include <vector>
#include <list>
#include <algorithm>
#include <limits>
#include <utility>
#include <list>

/**
 * @brief The TabouSearch class used to solve the vehicle routing problem using the tabou search algorithm
 * 
 */
class TabouSearch {
public:
    /**
     * @brief Construct a new TabouSearch object
     * 
     * @param config The configuration of the problem
     * @param num_iterations The number of iterations to run the tabou search
     * @param tabouDuration The duration of each move in the tabou list
     * @param verbose The verbose level
     */
    explicit TabouSearch(Config config, int num_iterations = 1000, int tabouDuration = 10, int verbose = 0);

    /**
     * @brief Initialize the solution with nearest neighbor and then run the tabou search algorithm
     * 
     * @return float The cost of the best solution found
     */
    float run();

    /**
     * @brief Get the best solution found
     * 
     * @return vector<vector<int>> The best solution found
     */
    vector<vector<int>> getBestSolution() const;

private:
    Config config;
    vector<vector<int>> currentSolution;
    vector<vector<int>> bestSolution;
    list<pair<pair<pair<int, int>, pair<int, int>>, int>> tabouList;
    float bestCost = numeric_limits<float>::max();
    int num_iterations;
    int tabouDuration;
    int tabouListMaxSize = 10;
    int verbose;

    /**
     * @brief Generate an initial solution using the nearest neighbor algorithm
     * 
     * @return vector<vector<int>> The initial solution
     */
    vector<vector<int>> generateInitialSolution();

    /**
     * @brief Calculate the cost of a solution returning 1e6 if the solution is invalid
     * 
     * @param solution The solution to calculate the cost
     * @return float The cost of the solution
     */
    float calculateCost(const vector<vector<int>>& solution);

    /**
     * @brief Generate the candidate moves for the tabou search
     * 
     * @param solution The solution to generate the candidate moves
     * @return vector<pair<pair<int, int>, pair<int, int>>> The candidate moves
     */
    vector<pair<pair<int, int>, pair<int, int>>> generateCandidateMoves(const vector<vector<int>>& solution);

    /**
     * @brief Display the solution
     * 
     * @param solution The solution to display
     */
    void displaySolution(const vector<vector<int>>& solution) const;

    /**
     * @brief Check if we can assign a client to a long term vehicle
     * 
     * @param tour The current tour of the vehicle
     * @param client The client to assign
     * @param vehicle The vehicle to assign
     * @return bool True if we can assign the client to the vehicle, false otherwise
     */
    bool canAssignLongTerm(const vector<int>& tour, int client, int vehicle);

    /**
     * @brief Check if we can assign a client to a short term vehicle
     * 
     * @param tour The current tour of the vehicle
     * @param client The client to assign
     * @param vehicle The vehicle to assign
     * @return bool True if we can assign the client to the vehicle, false otherwise
     */
    bool canAssignShortTerm(const vector<int> &tour, int client, int vehicle);

    /**
     * @brief Apply a move to the current best solution
     * 
     * @param move The move to apply
     * @return vector<vector<int>> The new solution
     */
    std::vector<std::vector<int>> applyMove(pair<pair<int, int>, pair<int, int>> move);

    /**
     * @brief Check if a move is tabou
     * 
     * @param move The move to check
     * @return bool True if the move is tabou, false otherwise
     */
    bool isTabou(pair<pair<int, int>, pair<int, int>> move);

    /**
     * @brief Add a move to the tabou list
     * 
     * @param move The move to add
     */
    void addToTabouList(pair<pair<int, int>, pair<int, int>> move);

    /**
     * @brief Decrement the tabou list
     */
    void decrementTabouList();

    /**
     * @brief Update the tabou list by adding a move and decrementing the tabou list
     * 
     * @param move The move to update
     */
    void updateTabouList(pair<pair<int, int>, pair<int, int>> move);
};