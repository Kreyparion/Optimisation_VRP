#include <iostream>
#include <chrono>
#include "CPLEX_implem.cpp"
#include "config.h"
#include "config.cpp"
#include "exact_solver.cpp"
#include "tabou_search.cpp"
#include <string>


int main(int argc, char *argv[]){

    // Check if an argument is provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <table_number[1:17]> <verbose{0,1,2}>\n";
        return 1; // Exit with error code if no argument is provided
    }
    int tab_number = std::stoi(argv[1]); // The first argument is the config file path
    int verbose = std::stoi(argv[2]); // The second argument is the verbose level

    auto start = std::chrono::high_resolution_clock::now();

    // Import the config file
    Config config = getConfig(tab_number, verbose);
    auto end_import = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_import = end_import - start;
    std::cout << "Data importation done in " << elapsed_import.count() << " s" << std::endl;

/*    // Solve the problem with the exact solver
    float exact_solver_score = exact_solver(config, verbose);
    auto end_exact_solver = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_exact_solver = end_exact_solver - end_import;
    std::cout << "Exact Algorithm Result: " << exact_solver_score << " in " << elapsed_exact_solver.count() << " s" << std::endl;

    // Solve the problem with the CPLEX solver
    float CPlex_score = opti(config, verbose);
    auto end_CPlex = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_CPlex = end_CPlex - end_exact_solver;
    std::cout << "CPLEX  Solver  Result : " << CPlex_score << " in " << elapsed_CPlex.count() << " s" << std::endl;*/

    // Solve the problem with the tabou search
    float Tabou_score = TabouSearch(config).run();
    auto end_Tabou = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_Tabou = end_Tabou - end_import;
    std::cout << "Tabou  Heuristic  Result : " << Tabou_score << " in " << elapsed_Tabou.count() << " s" << std::endl;

    return 0;
}
