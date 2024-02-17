#include <iostream>
#include "CPLEX_implem.cpp"
#include "config.h"
#include "config.cpp"
#include "plot_graph.cpp"
#include "heuristic.cpp"
#include <string>


int main(int argc, char *argv[]){

    // Check if an argument is provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <tab_number[1:17]> <verbose{1,2,3}>\n";
        return 1; // Exit with error code if no argument is provided
    }
    
    int tab_number = std::stoi(argv[1]); // The first argument is the config file path
    int verbose = std::stoi(argv[2]); // The second argument is the verbose level

    Config config = getConfig(tab_number, verbose);
    solve_heuristic(config, verbose);
    /*opti(config, verbose);*/

    return 0;
}
