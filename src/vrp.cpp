#include <iostream>
#include "CPLEX_implem.cpp"
#include "config.h"
#include "config.cpp"
#include "plot_graph.cpp"
#include "heuristic.cpp"
#include <string>

#include <yaml-cpp/yaml.h>

    
int main(int argc, char *argv[]){

    // Check if an argument is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>\n";
        return 1; // Exit with error code if no argument is provided
    }
    
    int tab_number = std::stoi(argv[1]); // The first argument is the config file path

    Config config = import_data(tab_number); // Adjust this part to use configFilePath
    solve_heuristic(config);
    opti(config);

    return 0;
}