#include <iostream>
#include <chrono>
#include "vrp.h"
#include "CPLEX_implem.cpp"
#include "config.h"
#include "config.cpp"
#include "exact_solver.h"
#include "exact_solver.cpp"
#include "heuristic.h"
#include "heuristic.cpp"
#include "tabou_search.cpp"
#include <string>

using namespace std;

int main(int argc, char *argv[]){

    // Check if an argument is provided
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <table_number[1:17]> <verbose{0,1,2}>\n";
        return 1; // Exit with error code if no argument is provided
    }
    int tab_number = stoi(argv[1]); // The first argument is the config file path
    int verbose = stoi(argv[2]); // The second argument is the verbose level

    auto start = chrono::high_resolution_clock::now();


    // Import the config file
    Config config = getConfig(tab_number, verbose);
    auto end_import = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_import = end_import - start;
    cout << "Data importation done in " << elapsed_import.count() << " s" << endl;

    // Solve the problem with the heuristic solver
    int max_time = 60;
    Solution heuristic_solver_solution = heuristic_solver(config, max_time, verbose);
    std::cout << heuristic_solver_solution << std::endl;
    float heuristic_solver_score = compute_and_check_solution(config, heuristic_solver_solution);
    auto end_heuristic_solver = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_heuristic_solver = end_heuristic_solver - end_import;
    std::cout << "Heuristic Algorithm Result: " << heuristic_solver_score << " in " << elapsed_heuristic_solver.count() << " s" << std::endl;

    // Solve the problem with the tabou search
    TabouSearch tabou = TabouSearch(config, 1000, 10, verbose);
    float Tabou_score = tabou.run();
    Solution Tabou_solution = tabou.getBestSolution();
    std::cout << Tabou_solution << std::endl;
    auto end_Tabou = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_Tabou = end_Tabou - end_heuristic_solver;
    cout << "Tabou  Heuristic  Result : " << Tabou_score << " in " << elapsed_Tabou.count() << " s" << endl;

    int n = config.nbVertex;
    if(n < 30){
        // Solve the problem with the exact solver
        Solution exact_solution = exact_solver(config, verbose);
        std::cout << exact_solution << std::endl;
        float exact_solver_score = compute_and_check_solution(config, exact_solution);
        auto end_exact_solver = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_exact_solver = end_exact_solver - end_Tabou;
        std::cout << "Exact Algorithm Result: " << exact_solver_score << " in " << elapsed_exact_solver.count() << " s" << std::endl;

        if(n < 17){
            // Solve the problem with the CPLEX solver
            Solution CPlex_solution = opti(config, verbose);
            std::cout << CPlex_solution << std::endl;
            float CPlex_score = compute_and_check_solution(config, CPlex_solution);
            auto end_CPlex = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_CPlex = end_CPlex - end_exact_solver;
            std::cout << "CPLEX  Solver  Result : " << CPlex_score << " in " << elapsed_CPlex.count() << " s" << std::endl;
        }
    }


    return 0;
}
