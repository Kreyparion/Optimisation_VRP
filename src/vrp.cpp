#include <iostream>
#include "CPLEX_implem.cpp"
#include "config.h"
#include "config.cpp"
#include "plot_graph.cpp"
#include "heuristic.cpp"


#include <yaml-cpp/yaml.h>

int main() {
      YAML::Node config_yaml = YAML::LoadFile("Optimisation_VRP/config.yml");
      // Ensure the YAML node for "number_tab" exists and can be converted to int
      std::cout << config_yaml["number_tab"].as<int>();
      if (config_yaml["number_tab"]) {
         int number_tab = config_yaml["number_tab"].as<int>();
         Config config = import_data(number_tab);
         solve_heuristic(config);
         opti(config);
      } else {
         // Handle the case where "number_tab" is not found or is not convertible to int
         std::cerr << "\"number_tab\" not found in the YAML file or is not an integer" << std::endl;
      }
   
    return 0;
}
