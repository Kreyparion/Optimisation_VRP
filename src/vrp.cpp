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
    std::cout << std::endl;
    cout << "Data from table " << tab_number << " imported in " << elapsed_import.count() << " s" << endl;


    // Solve the problem with the tabou search
    float best_score = 100000.0;
    Solution best_solution;
    for (int i = 0; i < 100; i++){
        TabouSearch tabou = TabouSearch(config, 1000, 10, verbose);
        float Tabou_score = tabou.run();
        Solution Tabou_solution = tabou.getBestSolution();
        if (Tabou_score < best_score){
            best_score = Tabou_score;
            best_solution = Tabou_solution;
        }
    }
    
    std::cout << "------------------ Tabou Algorithm --------------------" << std::endl;
    display_solution(config, best_solution);
    auto end_Tabou = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_Tabou = end_Tabou - end_import;
    cout << "Cost found : " << best_score << " in " << elapsed_Tabou.count() << " s" << endl;
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << std::endl;

    // Solve the problem with the heuristic solver
    int max_time = 60;
    Solution heuristic_solver_solution = heuristic_solver(config, max_time, verbose);
    std::cout << std::endl;
    std::cout << "----------------- Heuristic Algorithm -----------------" << std::endl;
    display_solution(config, heuristic_solver_solution);
    float heuristic_solver_score = compute_and_check_solution(config, heuristic_solver_solution);
    auto end_heuristic_solver = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_heuristic_solver = end_heuristic_solver - end_Tabou;
    std::cout << "Cost found : " << heuristic_solver_score << " in " << elapsed_heuristic_solver.count() << " s" << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << std::endl;

    int n = config.nbVertex;
    if(n < 30){
        // Solve the problem with the exact solver
        Solution exact_solution = exact_solver(config, verbose);
        std::cout << "------------------- Exact Algorithm -------------------" << std::endl;
        display_solution(config, exact_solution);
        float exact_solver_score = compute_and_check_solution(config, exact_solution);
        auto end_exact_solver = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_exact_solver = end_exact_solver - end_heuristic_solver;
        std::cout << "Cost found: " << exact_solver_score << " in " << elapsed_exact_solver.count() << " s" << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl;
        std::cout << std::endl;

        if(n < 17){
            // Solve the problem with the CPLEX solver
            Solution CPlex_solution = opti(config, verbose);
            std::cout << "-------------------- CPLEX  Solver --------------------" << std::endl;
            display_solution(config, CPlex_solution);
            float CPlex_score = compute_and_check_solution(config, CPlex_solution);
            auto end_CPlex = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_CPlex = end_CPlex - end_exact_solver;
            std::cout << "Cost found : " << CPlex_score << " in " << elapsed_CPlex.count() << " s" << std::endl;
            std::cout << "-------------------------------------------------------" << std::endl;
        }
    }


    return 0;
}
