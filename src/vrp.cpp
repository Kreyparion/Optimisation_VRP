#include <iostream>
#include "CPLEX_implem.cpp"
#include "config.h"
#include "config.cpp"


int main(){
   Config config = import_data();
   opti(config);

}