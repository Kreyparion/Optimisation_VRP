#include <iostream>
#include "CPLEX_implem.cpp"
#include "config.h"
#include "config.cpp"
#include "plot_graph.cpp"
#include "heuristic.cpp"

int main(){
   Config config = import_data();
   solve_heuristic(config);
   opti(config);
   return 0;
   }  