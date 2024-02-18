#pragma once

/** Adel **/
#include <vector>
#include <set>
#include <numeric>
#include "config.h"
#include "exact_solver.h"
#include <iostream>

// Structure pour une solution
struct TabouSolution;

float evaluate_permutation_tabou(Config& config, Permutation permutation);

// Fonction pour générer des solutions voisines
std::vector<TabouSolution> generateNeighbors(const TabouSolution &currentSolution, Config &config);

// Fonction de recherche tabou
TabouSolution tabouSearch(Config &config, const TabouSolution &initialSolution, int maxIterations, int tabouListSize);

TSPResults fill_results_rech_tabou(Config &config, int verbose);

float exact_solver_tabou(Config& config, int verbose=0);